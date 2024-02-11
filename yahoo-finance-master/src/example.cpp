#include "quote.hpp"
#include "time_utils.hpp"
#include "curl_utils.hpp"
#include <iostream>
#include<cmath>
#include <sstream>
#include<vector>
#include <algorithm>
using namespace std;

double func(vector<double> u){
    return pow(u[0], 2) + pow(u[1], 2);
}
double distance(vector<double> x, vector<double> y){
    return sqrt(pow(x[0]-y[0], 2) + pow(x[1]-y[1], 2));
}
vector<double> centroid(vector<double> x, vector<double> y){
    return {(x[0]+y[0])/2, (x[1]+y[1])/2};
}
vector<double> reflected(vector<double> w, vector<double> centroid){
    return {w[0] - 2*centroid[0], w[1] - 2*centroid[1]};
}
vector<double> expanded(vector<double> w, vector<double> centroid){
    return {w[0] - 4*centroid[0], w[1] - 4*centroid[1]};
}
vector<double> close_contract(vector<double> w, vector<double> centroid){
    return {w[0] - 0.5*centroid[0], w[1] - 0.5*centroid[1]};
}
vector<double> far_contract(vector<double> w, vector<double> centroid){
    return {w[0] - 1.5*centroid[0], w[1] - 1.5*centroid[1]};
}
vector<double> mean(vector<double> x, vector<double> y, vector<double> z){
    return {(x[0]+y[0]+z[0])/3, (x[1]+y[1]+z[1])/3};
}
vector<double> stand(vector<double> x, vector<double> y, vector<double> z){
    vector<double> average = mean(x, y, z);
    vector<double> deviation_x = {pow(x[0] - average[0], 2), pow(x[1] - average[1], 2)};
    vector<double> deviation_y = {pow(y[0] - average[0], 2), pow(y[1] - average[1], 2)};
    vector<double> deviation_z = {pow(z[0] - average[0], 2), pow(z[1] - average[1], 2)};
    vector<double> stand = {pow((deviation_x[0]+deviation_y[0]+deviation_z[0])/2, 0.5), pow((deviation_x[1]+deviation_y[1]+deviation_z[1])/2, 0.5)};
    return stand;
}
void print_vec(vector<double> x){
    
    cout << x[0] << " " << x[1] << endl;
}
bool operator<(vector<double> lhs, vector<double> rhs){
    if (func(lhs) < func(rhs)){
        return true;
    }
    return false;
}
bool operator==(vector<double> lhs, vector<double> rhs){
    if (func(lhs) == func(rhs)){
        return true;
    }
    return false;
}
bool operator>(vector<double> lhs, vector<double> rhs){
    if (func(lhs) > func(rhs)){
        return true;
    }
    return false;
}
void nelson_mead(){
    vector<double> u = {2, 0};
    vector<double> w = {1, 0};
    vector<double> v = {3, 0};
    // while (stand(u, v, w)[0] > 0.1 && stand(u, v, w)[0] > 0.1){
    vector<vector<double>> order = {u, v, w};
    std::sort(order.begin(), order.end());
    vector<double> std = {0.05, 0,05};
    // print_vec(stand(order[0], order[1], order[2]));
    while (stand(order[0], order[1], order[2]) > std) {
        cout << "urmom";
        break;
    }
    } 
}

int main() {
    // S&P 500
    nelson_mead();
    // Quote *snp500 = new Quote("SPY230130C00320000");
    // std::time_t period1 = dateToEpoch("2023-01-20");
    // cout << period1 << endl;
    // Get the historical spots from Yahoo Finance
    // snp500->getHistoricalSpots("2023-1-23", "2023-1-26", "1d");

    
    // Print the spots
    // snp500->printSpots();

    // Print a spot
    // try {
    //     Spot spot = snp500->getSpot("2023-1-27");
    //     std::cout << "Spot at 2017-12-01: " << std::endl;
    //     spot.printSpot();
    // } catch(const std::exception &e) {
    //     std::cerr << e.what() << std::endl;
    // }

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
    // delete snp500;
    // delete eurusd;
    // delete euraud;
    // cout << "hello world" << endl;
}
