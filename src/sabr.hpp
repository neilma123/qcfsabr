#pragma once

#include <vector>

namespace qe {

struct SabrParams {
    double alpha = 0.2;
    double beta = 1.0;
    double rho = 0.0;
    double nu = 0.5;
};

struct CalibrationResult {
    SabrParams params;
    double rmse = 0.0;
    int iterations = 0;
    bool converged = false;
};

double normal_cdf(double x);
double normal_pdf(double x);
double black76_price(double forward, double strike, double expiry,
                     double rate, double volatility, bool is_call);
double black76_delta(double forward, double strike, double expiry,
                     double rate, double volatility, bool is_call);
double black76_vega(double forward, double strike, double expiry,
                    double rate, double volatility);
double sabr_implied_vol(double forward, double strike, double expiry,
                        const SabrParams& params);

CalibrationResult calibrate_sabr(
    double forward,
    double expiry,
    const std::vector<double>& strikes,
    const std::vector<double>& market_vols,
    double beta = 1.0);

bool run_self_tests();

}  // namespace qe

