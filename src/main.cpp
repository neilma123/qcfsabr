#include "backtest.hpp"
#include "sabr.hpp"
#include "yahoo.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void usage() {
    std::cout <<
        "QuantEdge SABR statistical-arbitrage research tool\n\n"
        "Usage:\n"
        "  statarb self-test\n"
        "  statarb backtest --data DIR [--underlying SPXW] [--output FILE]\n"
        "  statarb yahoo SYMBOL START_DATE END_DATE\n\n"
        "Yahoo SYMBOL may be an underlying or OCC option symbol, e.g.\n"
        "SPY or SPY261218C00600000. Dates use YYYY-MM-DD.\n";
}

}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc < 2) { usage(); return 1; }
        const std::string command = argv[1];
        if (command == "self-test") return qe::run_self_tests() ? 0 : 1;
        if (command == "yahoo") {
            if (argc != 5) { usage(); return 1; }
            return qe::print_yahoo_history(argv[2], argv[3], argv[4]);
        }
        if (command == "backtest") {
            qe::BacktestConfig config;
            for (int i = 2; i < argc; ++i) {
                const std::string arg = argv[i];
                if (i + 1 >= argc) throw std::runtime_error("Missing value for " + arg);
                const std::string value = argv[++i];
                if (arg == "--data") config.data_directory = value;
                else if (arg == "--underlying") config.underlying = value;
                else if (arg == "--output") config.output_file = value;
                else if (arg == "--beta") config.beta = std::stod(value);
                else throw std::runtime_error("Unknown argument: " + arg);
            }
            if (config.data_directory.empty()) throw std::runtime_error("--data is required");
            if (config.output_file.empty()) config.output_file = "results/backtest_trades.csv";
            return qe::run_backtest(config);
        }
        usage();
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}

