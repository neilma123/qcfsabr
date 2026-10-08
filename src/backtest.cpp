#include "backtest.hpp"

#include "sabr.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace qe {
namespace {

struct OptionRow {
    std::string contract;
    std::string underlying;
    std::string expiration;
    std::string quote_date;
    std::string type;
    double strike = 0.0;
    double bid = 0.0;
    double ask = 0.0;
    int volume = 0;
    int open_interest = 0;
    double underlying_close = 0.0;
    double iv = 0.0;
    int iv_flag = -1;
    double delta = 0.0;
    double vega = 0.0;

    double mid() const { return 0.5 * (bid + ask); }
};

struct Trade {
    std::string entry_date;
    std::string exit_date;
    std::string expiration;
    OptionRow cheap;
    OptionRow rich;
    SabrParams params;
    double fit_rmse = 0.0;
    double cheap_residual = 0.0;
    double rich_residual = 0.0;
    double rich_quantity = 0.0;
    double gross_premium = 0.0;
    double mid_pnl = 0.0;
    double crossed_pnl = 0.0;
    double mid_return = 0.0;
    double crossed_return = 0.0;
};

std::vector<std::string> split_csv(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '"') {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"') {
                field.push_back('"');
                ++i;
            } else {
                quoted = !quoted;
            }
        } else if (c == ',' && !quoted) {
            fields.push_back(field);
            field.clear();
        } else {
            field.push_back(c);
        }
    }
    fields.push_back(field);
    return fields;
}

double number(const std::string& s, double fallback = 0.0) {
    if (s.empty()) return fallback;
    try { return std::stod(s); } catch (...) { return fallback; }
}

int integer(const std::string& s, int fallback = 0) {
    if (s.empty()) return fallback;
    try { return std::stoi(s); } catch (...) { return fallback; }
}

int serial_day(const std::string& date) {
    int year = 0, month = 0, day = 0;
    char dash1 = 0, dash2 = 0;
    std::istringstream input(date);
    input >> year >> dash1 >> month >> dash2 >> day;
    if (!input || dash1 != '-' || dash2 != '-') return 0;
    if (month <= 2) { year -= 1; month += 12; }
    return 365 * year + year / 4 - year / 100 + year / 400 +
           (153 * (month - 3) + 2) / 5 + day - 1;
}

std::vector<OptionRow> load_day(const std::filesystem::path& path,
                                const std::string& underlying) {
    std::ifstream input(path);
    if (!input) throw std::runtime_error("Unable to open " + path.string());
    std::vector<OptionRow> rows;
    std::string line;
    std::getline(input, line);
    while (std::getline(input, line)) {
        const auto f = split_csv(line);
        if (f.size() < 34 || f[1] != underlying || f[5] != "E" || f[24] != "PM") continue;
        OptionRow row;
        row.contract = f[0]; row.underlying = f[1]; row.expiration = f[2];
        row.type = f[3];
        row.strike = number(f[4]); row.quote_date = f[6]; row.bid = number(f[7]);
        row.ask = number(f[9]); row.volume = integer(f[12]); row.open_interest = integer(f[13]);
        row.underlying_close = number(f[23]); row.iv = number(f[27]); row.iv_flag = integer(f[28], -1);
        row.delta = number(f[29]); row.vega = number(f[32]);
        rows.push_back(std::move(row));
    }
    return rows;
}

bool liquid_for_entry(const OptionRow& row, const BacktestConfig& cfg) {
    if (row.type != "call" || row.bid <= 0.0 || row.ask <= row.bid || row.iv_flag != 0 || row.iv <= 0.0 ||
        row.vega <= 0.0 || row.underlying_close <= 0.0 ||
        row.open_interest < cfg.minimum_open_interest || row.volume < cfg.minimum_volume)
        return false;
    const double mid = row.mid();
    if (mid < cfg.minimum_option_mid || (row.ask - row.bid) / mid > cfg.maximum_relative_spread) return false;
    const double moneyness = row.strike / row.underlying_close;
    return moneyness >= 0.80 && moneyness <= 1.20;
}

double percentile(std::vector<double> values, double p);

double parity_forward(const std::vector<OptionRow>& rows, const std::string& expiration,
                      double expiry, double rate, double fallback) {
    std::map<double, double> calls;
    std::map<double, double> puts;
    for (const auto& row : rows) {
        if (row.expiration != expiration || row.bid <= 0.0 || row.ask <= row.bid) continue;
        if (row.type == "call") calls[row.strike] = row.mid();
        if (row.type == "put") puts[row.strike] = row.mid();
    }
    std::vector<double> forwards;
    for (const auto& [strike, call] : calls) {
        const auto put = puts.find(strike);
        if (put == puts.end()) continue;
        const double fwd = strike + std::exp(rate * expiry) * (call - put->second);
        if (fwd > fallback * 0.90 && fwd < fallback * 1.10) forwards.push_back(fwd);
    }
    return forwards.size() >= 3 ? percentile(std::move(forwards), 0.5)
                                : fallback * std::exp(rate * expiry);
}

double sample_stddev(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    const double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sum = 0.0;
    for (double x : values) sum += (x - mean) * (x - mean);
    return std::sqrt(sum / static_cast<double>(values.size() - 1));
}

double percentile(std::vector<double> values, double p) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    const double index = p * static_cast<double>(values.size() - 1);
    const auto lo = static_cast<std::size_t>(std::floor(index));
    const auto hi = static_cast<std::size_t>(std::ceil(index));
    const double w = index - static_cast<double>(lo);
    return values[lo] * (1.0 - w) + values[hi] * w;
}

void write_trade_header(std::ofstream& out) {
    out << "entry_date,exit_date,expiration,cheap_contract,rich_contract,spot,alpha,beta,rho,nu,fit_rmse,"
           "cheap_market_iv,cheap_model_iv,rich_market_iv,rich_model_iv,rich_quantity,gross_premium,"
           "mid_pnl,crossed_pnl,mid_return,crossed_return\n";
}

void write_trade(std::ofstream& out, const Trade& t) {
    out << std::setprecision(10) << t.entry_date << ',' << t.exit_date << ',' << t.expiration << ','
        << t.cheap.contract << ',' << t.rich.contract << ',' << t.cheap.underlying_close << ','
        << t.params.alpha << ',' << t.params.beta << ',' << t.params.rho << ',' << t.params.nu << ','
        << t.fit_rmse << ',' << t.cheap.iv << ',' << t.cheap.iv - t.cheap_residual << ','
        << t.rich.iv << ',' << t.rich.iv - t.rich_residual << ',' << t.rich_quantity << ','
        << t.gross_premium << ',' << t.mid_pnl << ',' << t.crossed_pnl << ','
        << t.mid_return << ',' << t.crossed_return << '\n';
}

}  // namespace

int run_backtest(const BacktestConfig& cfg) {
    if (!std::filesystem::is_directory(cfg.data_directory)) {
        std::cerr << "Data directory not found: " << cfg.data_directory << '\n';
        return 2;
    }
    std::vector<std::filesystem::path> files;
    for (const auto& item : std::filesystem::directory_iterator(cfg.data_directory)) {
        if (item.is_regular_file() && item.path().filename().string().find("_options.csv") != std::string::npos)
            files.push_back(item.path());
    }
    std::sort(files.begin(), files.end());
    if (files.size() < 2) {
        std::cerr << "Need at least two daily option-chain files.\n";
        return 2;
    }

    if (!cfg.output_file.parent_path().empty())
        std::filesystem::create_directories(cfg.output_file.parent_path());
    std::ofstream output(cfg.output_file);
    if (!output) throw std::runtime_error("Unable to create " + cfg.output_file.string());
    write_trade_header(output);

    std::vector<Trade> trades;
    int skipped_no_surface = 0, skipped_no_exit = 0, skipped_weak_signal = 0;
    for (std::size_t day = 0; day + 1 < files.size(); ++day) {
        const auto today = load_day(files[day], cfg.underlying);
        const auto tomorrow = load_day(files[day + 1], cfg.underlying);
        std::unordered_map<std::string, OptionRow> exits;
        for (const auto& row : tomorrow) {
            if (row.bid > 0.0 && row.ask > row.bid) exits[row.contract] = row;
        }

        std::map<std::string, std::vector<OptionRow>> by_expiry;
        for (const auto& row : today) {
            const int dte = serial_day(row.expiration) - serial_day(row.quote_date);
            if (dte >= cfg.minimum_dte && dte <= cfg.maximum_dte && liquid_for_entry(row, cfg))
                by_expiry[row.expiration].push_back(row);
        }
        auto chosen = by_expiry.end();
        int best_distance = std::numeric_limits<int>::max();
        for (auto it = by_expiry.begin(); it != by_expiry.end(); ++it) {
            if (it->second.size() < 9) continue;
            const int dte = serial_day(it->first) - serial_day(it->second.front().quote_date);
            const int distance = std::abs(dte - 35);
            if (distance < best_distance) { best_distance = distance; chosen = it; }
        }
        if (chosen == by_expiry.end()) { ++skipped_no_surface; continue; }

        auto surface = chosen->second;
        std::sort(surface.begin(), surface.end(), [](const OptionRow& a, const OptionRow& b) { return a.strike < b.strike; });
        const double expiry = static_cast<double>(serial_day(chosen->first) - serial_day(surface.front().quote_date)) / 365.0;
        const double spot = surface.front().underlying_close;
        const double forward = parity_forward(today, chosen->first, expiry, cfg.risk_free_rate, spot);
        std::vector<double> strikes, vols;
        for (const auto& row : surface) { strikes.push_back(row.strike); vols.push_back(row.iv); }
        const auto fit = calibrate_sabr(forward, expiry, strikes, vols, cfg.beta);
        if (!std::isfinite(fit.rmse) || fit.rmse > 0.15) { ++skipped_no_surface; continue; }

        std::vector<double> residuals;
        residuals.reserve(surface.size());
        for (const auto& row : surface)
            residuals.push_back(row.iv - sabr_implied_vol(forward, row.strike, expiry, fit.params));
        const auto min_it = std::min_element(residuals.begin(), residuals.end());
        const auto max_it = std::max_element(residuals.begin(), residuals.end());
        const std::size_t cheap_index = static_cast<std::size_t>(std::distance(residuals.begin(), min_it));
        const std::size_t rich_index = static_cast<std::size_t>(std::distance(residuals.begin(), max_it));
        const double residual_scale = sample_stddev(residuals);
        if (residual_scale <= 0.0 || -*min_it < residual_scale || *max_it < residual_scale) {
            ++skipped_weak_signal;
            continue;
        }
        const auto cheap_exit = exits.find(surface[cheap_index].contract);
        const auto rich_exit = exits.find(surface[rich_index].contract);
        if (cheap_exit == exits.end() || rich_exit == exits.end()) { ++skipped_no_exit; continue; }

        Trade trade;
        trade.entry_date = surface.front().quote_date;
        trade.exit_date = cheap_exit->second.quote_date;
        trade.expiration = chosen->first;
        trade.cheap = surface[cheap_index];
        trade.rich = surface[rich_index];
        trade.params = fit.params;
        trade.fit_rmse = fit.rmse;
        trade.cheap_residual = *min_it;
        trade.rich_residual = *max_it;
        trade.rich_quantity = trade.cheap.vega / trade.rich.vega;
        const double q_cheap = 1.0;
        const double q_rich = -trade.rich_quantity;
        trade.gross_premium = 100.0 * (std::abs(q_cheap) * trade.cheap.mid() + std::abs(q_rich) * trade.rich.mid());
        const double spot_change = cheap_exit->second.underlying_close - trade.cheap.underlying_close;
        const double hedge_shares = -100.0 * (q_cheap * trade.cheap.delta + q_rich * trade.rich.delta);
        const double hedge_pnl = hedge_shares * spot_change;
        trade.mid_pnl = 100.0 * (q_cheap * (cheap_exit->second.mid() - trade.cheap.mid()) +
                                 q_rich * (rich_exit->second.mid() - trade.rich.mid())) + hedge_pnl;
        trade.crossed_pnl = 100.0 * (q_cheap * (cheap_exit->second.bid - trade.cheap.ask) +
                                     trade.rich_quantity * (trade.rich.bid - rich_exit->second.ask)) + hedge_pnl;
        trade.mid_return = trade.mid_pnl / trade.gross_premium;
        trade.crossed_return = trade.crossed_pnl / trade.gross_premium;
        trades.push_back(trade);
        write_trade(output, trade);
    }

    if (trades.empty()) {
        std::cerr << "No trades passed the filters.\n";
        return 3;
    }
    std::vector<double> mid_returns, crossed_returns;
    double total_mid_pnl = 0.0, total_crossed_pnl = 0.0;
    int mid_wins = 0, crossed_wins = 0;
    double mid_equity = 1.0, crossed_equity = 1.0, mid_peak = 1.0, crossed_peak = 1.0;
    double mid_drawdown = 0.0, crossed_drawdown = 0.0;
    for (const auto& trade : trades) {
        mid_returns.push_back(trade.mid_return);
        crossed_returns.push_back(trade.crossed_return);
        total_mid_pnl += trade.mid_pnl; total_crossed_pnl += trade.crossed_pnl;
        if (trade.mid_pnl > 0.0) ++mid_wins;
        if (trade.crossed_pnl > 0.0) ++crossed_wins;
        mid_equity *= std::max(0.01, 1.0 + trade.mid_return);
        crossed_equity *= std::max(0.01, 1.0 + trade.crossed_return);
        mid_peak = std::max(mid_peak, mid_equity); crossed_peak = std::max(crossed_peak, crossed_equity);
        mid_drawdown = std::min(mid_drawdown, mid_equity / mid_peak - 1.0);
        crossed_drawdown = std::min(crossed_drawdown, crossed_equity / crossed_peak - 1.0);
    }
    const auto mean = [](const std::vector<double>& x) { return std::accumulate(x.begin(), x.end(), 0.0) / x.size(); };
    const double mid_sharpe = sample_stddev(mid_returns) > 0.0 ? mean(mid_returns) / sample_stddev(mid_returns) * std::sqrt(252.0) : 0.0;
    const double crossed_sharpe = sample_stddev(crossed_returns) > 0.0 ? mean(crossed_returns) / sample_stddev(crossed_returns) * std::sqrt(252.0) : 0.0;

    std::cout << std::fixed << std::setprecision(4)
              << "SABR relative-value backtest\n"
              << "Data: " << files.front().stem().string().substr(0, 10) << " through "
              << files.back().stem().string().substr(0, 10) << " | Underlying: " << cfg.underlying << '\n'
              << "Trades: " << trades.size() << " | no surface: " << skipped_no_surface
              << " | no next-day quote: " << skipped_no_exit << " | weak signal: " << skipped_weak_signal << '\n'
              << "Midpoint     total P&L: $" << total_mid_pnl << " | mean daily return: " << mean(mid_returns) * 100.0
              << "% | Sharpe: " << mid_sharpe << " | win rate: " << 100.0 * mid_wins / trades.size()
              << "% | max DD: " << 100.0 * mid_drawdown << "%\n"
              << "Bid/ask     total P&L: $" << total_crossed_pnl << " | mean daily return: " << mean(crossed_returns) * 100.0
              << "% | Sharpe: " << crossed_sharpe << " | win rate: " << 100.0 * crossed_wins / trades.size()
              << "% | max DD: " << 100.0 * crossed_drawdown << "%\n"
              << "Median gross premium per paired trade: $";
    std::vector<double> premiums;
    for (const auto& t : trades) premiums.push_back(t.gross_premium);
    std::cout << percentile(premiums, 0.5) << "\nTrade log: " << cfg.output_file << '\n';
    return 0;
}

}  // namespace qe
