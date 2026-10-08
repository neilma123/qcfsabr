#include "sabr.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>

namespace qe {
namespace {

constexpr double kPi = 3.14159265358979323846;

double clamp(double x, double lo, double hi) {
    return std::max(lo, std::min(hi, x));
}

SabrParams decode(const std::array<double, 3>& x, double beta) {
    SabrParams p;
    p.alpha = std::exp(clamp(x[0], -12.0, 3.0));
    p.beta = beta;
    p.rho = std::tanh(x[1]) * 0.999;
    p.nu = std::exp(clamp(x[2], -12.0, 3.0));
    return p;
}

double objective(const std::array<double, 3>& x, double forward, double expiry,
                 const std::vector<double>& strikes,
                 const std::vector<double>& vols, double beta) {
    const auto p = decode(x, beta);
    double sum = 0.0;
    for (std::size_t i = 0; i < strikes.size(); ++i) {
        const double model = sabr_implied_vol(forward, strikes[i], expiry, p);
        if (!std::isfinite(model) || model <= 0.0 || model > 5.0) return 1e12;
        const double e = model - vols[i];
        sum += e * e;
    }
    return sum / static_cast<double>(strikes.size());
}

}  // namespace

double normal_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double normal_pdf(double x) {
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * kPi);
}

double black76_price(double forward, double strike, double expiry,
                     double rate, double volatility, bool is_call) {
    if (forward <= 0.0 || strike <= 0.0 || expiry < 0.0 || volatility < 0.0)
        return std::numeric_limits<double>::quiet_NaN();
    const double discount = std::exp(-rate * expiry);
    if (expiry == 0.0 || volatility == 0.0) {
        const double intrinsic = is_call ? forward - strike : strike - forward;
        return discount * std::max(0.0, intrinsic);
    }
    const double root_t = std::sqrt(expiry);
    const double d1 = (std::log(forward / strike) + 0.5 * volatility * volatility * expiry)
                    / (volatility * root_t);
    const double d2 = d1 - volatility * root_t;
    if (is_call) return discount * (forward * normal_cdf(d1) - strike * normal_cdf(d2));
    return discount * (strike * normal_cdf(-d2) - forward * normal_cdf(-d1));
}

double black76_delta(double forward, double strike, double expiry,
                     double rate, double volatility, bool is_call) {
    if (expiry <= 0.0 || volatility <= 0.0) {
        const double raw = is_call ? (forward > strike ? 1.0 : 0.0)
                                   : (forward < strike ? -1.0 : 0.0);
        return std::exp(-rate * expiry) * raw;
    }
    const double d1 = (std::log(forward / strike) + 0.5 * volatility * volatility * expiry)
                    / (volatility * std::sqrt(expiry));
    return std::exp(-rate * expiry) * (is_call ? normal_cdf(d1) : normal_cdf(d1) - 1.0);
}

double black76_vega(double forward, double strike, double expiry,
                    double rate, double volatility) {
    if (expiry <= 0.0 || volatility <= 0.0) return 0.0;
    const double d1 = (std::log(forward / strike) + 0.5 * volatility * volatility * expiry)
                    / (volatility * std::sqrt(expiry));
    return std::exp(-rate * expiry) * forward * normal_pdf(d1) * std::sqrt(expiry);
}

double sabr_implied_vol(double forward, double strike, double expiry,
                        const SabrParams& p) {
    if (forward <= 0.0 || strike <= 0.0 || expiry < 0.0 || p.alpha <= 0.0 ||
        p.nu <= 0.0 || p.beta < 0.0 || p.beta > 1.0 || std::abs(p.rho) >= 1.0)
        return std::numeric_limits<double>::quiet_NaN();

    const double one_minus_beta = 1.0 - p.beta;
    const double fk = forward * strike;
    const double fk_beta = std::pow(fk, 0.5 * one_minus_beta);
    const double log_fk = std::log(forward / strike);
    const double log2 = log_fk * log_fk;
    const double denominator = fk_beta *
        (1.0 + one_minus_beta * one_minus_beta * log2 / 24.0 +
         std::pow(one_minus_beta, 4) * log2 * log2 / 1920.0);
    const double correction = 1.0 + expiry * (
        one_minus_beta * one_minus_beta * p.alpha * p.alpha /
            (24.0 * std::pow(fk, one_minus_beta)) +
        0.25 * p.rho * p.beta * p.nu * p.alpha / fk_beta +
        (2.0 - 3.0 * p.rho * p.rho) * p.nu * p.nu / 24.0);

    double z_over_x = 1.0;
    if (std::abs(log_fk) > 1e-10) {
        const double z = (p.nu / p.alpha) * fk_beta * log_fk;
        if (std::abs(z) > 1e-8) {
            const double inside = std::max(0.0, 1.0 - 2.0 * p.rho * z + z * z);
            const double ratio = (std::sqrt(inside) + z - p.rho) / (1.0 - p.rho);
            if (ratio <= 0.0) return std::numeric_limits<double>::quiet_NaN();
            const double xz = std::log(ratio);
            if (std::abs(xz) > 1e-12) z_over_x = z / xz;
        }
    }
    return (p.alpha / denominator) * z_over_x * correction;
}

CalibrationResult calibrate_sabr(double forward, double expiry,
                                 const std::vector<double>& strikes,
                                 const std::vector<double>& market_vols,
                                 double beta) {
    CalibrationResult result;
    result.params.beta = beta;
    if (strikes.size() != market_vols.size() || strikes.size() < 4 ||
        forward <= 0.0 || expiry <= 0.0) return result;

    std::size_t atm = 0;
    for (std::size_t i = 1; i < strikes.size(); ++i) {
        if (std::abs(strikes[i] - forward) < std::abs(strikes[atm] - forward)) atm = i;
    }
    const double initial_alpha = clamp(market_vols[atm] * std::pow(forward, 1.0 - beta), 1e-4, 3.0);
    using Vertex = std::pair<std::array<double, 3>, double>;
    std::array<double, 3> x0{std::log(initial_alpha), std::atanh(-0.15), std::log(0.5)};
    std::array<Vertex, 4> simplex;
    simplex[0].first = x0;
    for (int i = 1; i < 4; ++i) {
        simplex[i].first = x0;
        simplex[i].first[static_cast<std::size_t>(i - 1)] += (i == 2 ? 0.5 : 0.35);
    }
    for (auto& v : simplex)
        v.second = objective(v.first, forward, expiry, strikes, market_vols, beta);

    constexpr int max_iterations = 500;
    for (int iteration = 0; iteration < max_iterations; ++iteration) {
        std::sort(simplex.begin(), simplex.end(),
                  [](const Vertex& a, const Vertex& b) { return a.second < b.second; });
        result.iterations = iteration + 1;
        if (std::sqrt(simplex.back().second) - std::sqrt(simplex.front().second) < 1e-9) {
            result.converged = true;
            break;
        }
        std::array<double, 3> centroid{};
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) centroid[static_cast<std::size_t>(j)] += simplex[static_cast<std::size_t>(i)].first[static_cast<std::size_t>(j)] / 3.0;
        auto move = [&](double scale) {
            std::array<double, 3> out{};
            for (int j = 0; j < 3; ++j)
                out[static_cast<std::size_t>(j)] = centroid[static_cast<std::size_t>(j)] +
                    scale * (centroid[static_cast<std::size_t>(j)] - simplex[3].first[static_cast<std::size_t>(j)]);
            return out;
        };
        const auto reflected = move(1.0);
        const double fr = objective(reflected, forward, expiry, strikes, market_vols, beta);
        if (fr < simplex[0].second) {
            const auto expanded = move(2.0);
            const double fe = objective(expanded, forward, expiry, strikes, market_vols, beta);
            simplex[3] = fe < fr ? Vertex{expanded, fe} : Vertex{reflected, fr};
        } else if (fr < simplex[2].second) {
            simplex[3] = {reflected, fr};
        } else {
            const auto contracted = move(0.5);
            const double fc = objective(contracted, forward, expiry, strikes, market_vols, beta);
            if (fc < simplex[3].second) {
                simplex[3] = {contracted, fc};
            } else {
                for (int i = 1; i < 4; ++i) {
                    for (int j = 0; j < 3; ++j)
                        simplex[static_cast<std::size_t>(i)].first[static_cast<std::size_t>(j)] =
                            0.5 * (simplex[0].first[static_cast<std::size_t>(j)] + simplex[static_cast<std::size_t>(i)].first[static_cast<std::size_t>(j)]);
                    simplex[static_cast<std::size_t>(i)].second = objective(simplex[static_cast<std::size_t>(i)].first, forward, expiry, strikes, market_vols, beta);
                }
            }
        }
    }
    std::sort(simplex.begin(), simplex.end(),
              [](const Vertex& a, const Vertex& b) { return a.second < b.second; });
    result.params = decode(simplex[0].first, beta);
    result.rmse = std::sqrt(simplex[0].second);
    return result;
}

bool run_self_tests() {
    bool ok = true;
    const double call = black76_price(100.0, 100.0, 1.0, 0.0, 0.2, true);
    const double put = black76_price(100.0, 100.0, 1.0, 0.0, 0.2, false);
    ok = ok && std::abs(call - 7.965567) < 1e-5 && std::abs(call - put) < 1e-12;

    const SabrParams truth{0.22, 1.0, -0.32, 0.75};
    std::vector<double> strikes{75, 82.5, 90, 95, 100, 105, 110, 117.5, 125};
    std::vector<double> vols;
    for (double strike : strikes) vols.push_back(sabr_implied_vol(100.0, strike, 0.5, truth));
    const auto fit = calibrate_sabr(100.0, 0.5, strikes, vols, 1.0);
    ok = ok && fit.rmse < 1e-5 && std::abs(fit.params.alpha - truth.alpha) < 0.01 &&
         std::abs(fit.params.rho - truth.rho) < 0.03 && std::abs(fit.params.nu - truth.nu) < 0.03;
    std::cout << (ok ? "SELF-TEST PASS" : "SELF-TEST FAIL")
              << " | calibration RMSE=" << fit.rmse
              << " alpha=" << fit.params.alpha << " rho=" << fit.params.rho
              << " nu=" << fit.params.nu << '\n';
    return ok;
}

}  // namespace qe

