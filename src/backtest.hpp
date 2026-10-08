#pragma once

#include <filesystem>
#include <string>

namespace qe {

struct BacktestConfig {
    std::filesystem::path data_directory;
    std::filesystem::path output_file;
    std::string underlying = "SPXW";
    int minimum_dte = 21;
    int maximum_dte = 60;
    int minimum_open_interest = 100;
    int minimum_volume = 10;
    double maximum_relative_spread = 0.20;
    double minimum_option_mid = 1.0;
    double beta = 1.0;
    double risk_free_rate = 0.03;
};

int run_backtest(const BacktestConfig& config);

}  // namespace qe

