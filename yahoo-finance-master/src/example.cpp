#include "quote.hpp"
#include "time_utils.hpp"
#include "curl_utils.hpp"
#include <iostream>
#include <sstream>
using namespace std;
int main() {
    // S&P 500
    Quote *snp500 = new Quote("SPY230130C00320000");
    std::time_t period1 = dateToEpoch("2023-01-20");
    cout << period1 << endl;
    // Get the historical spots from Yahoo Finance
    snp500->getHistoricalSpots("2023-1-23", "2023-1-26", "1d");

    
    // Print the spots
    snp500->printSpots();

    // Print a spot
    try {
        Spot spot = snp500->getSpot("2023-1-27");
        std::cout << "Spot at 2017-12-01: " << std::endl;
        spot.printSpot();
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    // S&P 500
    // Quote *snp500 = new Quote("^GSPC");

    // // Get the historical spots from Yahoo Finance
    // snp500->getHistoricalSpots("2017-12-01", "2017-12-31", "1d");

    // // Print the spots
    // snp500->printSpots();

    // // Print a spot
    // try {
    //     Spot spot = snp500->getSpot("2017-12-01");
    //     std::cout << "Spot at 2017-12-01: " << std::endl;
    //     spot.printSpot();
    // } catch(const std::exception &e) {
    //     std::cerr << e.what() << std::endl;
    // }
    // // Get the historical EUR/USD rates
    // Quote *eurusd = new Quote("EURUSD=X");
    // eurusd->getHistoricalSpots("2018-01-01", "2018-01-10", "1d");
    // std::cout << "EUR/USD rates between 2018-01-01 and 2018-01-10" << std::endl;
    // eurusd->printSpots();

    // // Get the historical EUR/AUD rates
    // Quote *euraud = new Quote("EURAUD=X");
    // euraud->getHistoricalSpots("2018-01-01", "2018-01-10", "1d");
    // std::cout << "EUR/AUD rates between 2018-01-01 and 2018-01-10" << std::endl;
    // euraud->printSpots();

    // Free memory
    delete snp500;
    // delete eurusd;
    // delete euraud;
    cout << "hello world" << endl;
}
