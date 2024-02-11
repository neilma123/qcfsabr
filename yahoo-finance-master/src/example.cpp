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
    return pow(u[0] - 1, 2) + pow(u[1] + 5, 2);
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
    vector<double> u = {2, 1};
    vector<double> w = {1, 2};
    vector<double> v = {3, 3};
    // while (stand(u, v, w)[0] > 0.1 && stand(u, v, w)[0] > 0.1){
    vector<vector<double>> order = {u, v, w};
    std::sort(order.begin(), order.end());
    vector<double> std = {0.005, 0.005};
    print_vec(stand(order[0], order[1], order[2])); 
    while (stand(order[0], order[1], order[2])[0] > std[0] || stand(order[0], order[1], order[2])[1] > std[1]){
        // cout << "urmom" << endl;
        std::sort(order.begin(), order.end());
        vector<double> centroid_1 = centroid(order[0], order[1]);
        vector<double> reflected_point = reflected(order[2], centroid_1);
        cout << "reflected point is ";
        print_vec(reflected_point);
        if (reflected_point < order[2] && reflected_point >= order[0]){
            order[2] = reflected_point;
            cout << "reflected ";
            print_vec(order[2]);
            cout << "to: "; 
            print_vec(reflected_point);
            cout << endl;
            print_vec(order[0]);
            print_vec(order[1]);
            print_vec(order[2]);
            continue;
        }
        if (reflected_point < order[0]){
            vector<double> expanded_point = expanded(order[2], reflected_point);
            cout << "expanded point is ";
            print_vec(expanded_point);
            if (expanded_point < reflected_point){
                order[2] = expanded_point;
                cout << "expanded";
                print_vec(order[2]);
                cout << "to: ";
                print_vec(expanded_point);
                cout << endl;
                continue;
            }
            else {
                order[2] = reflected_point;
                print_vec(order[0]);
                print_vec(order[1]);
                print_vec(order[2]);
                cout << "reflected ";
                print_vec(order[2]);
                cout << "to: ";
                print_vec(reflected_point);
                cout << endl;
                continue;
            }
        }
        if (reflected_point < order[2]){
            vector<double> far_contracted_point = far_contract(order[2], centroid_1);
            cout << "far contracted point is ";
            print_vec(far_contracted_point);
            if (far_contracted_point <= reflected_point){
                order[2] = far_contracted_point;
                cout << "far contracted ";
                print_vec(order[2]);
                cout << "to: ";
                print_vec(far_contracted_point);
                cout << endl;
                continue;
            }
            else {
                vector<double> close_contracted_point = close_contract(order[2], centroid_1);
                cout << "close contracted point is ";
                print_vec(close_contracted_point);
                cout << "made it here";
                order[2] = close_contracted_point;
                order[1] = close_contracted_point;
                print_vec(order[0]);
                cout << endl;
                print_vec(order[1]);
                cout << endl;
                print_vec(order[2]);
                cout << endl;
            }
        }

        cout << "still going" << endl;
        print_vec(stand(order[0], order[1], order[2]));
        print_vec(order[0]);
        print_vec(order[1]);
        print_vec(order[2]);
        std::sort(order.begin(), order.end());
    }
    print_vec(order[0]);
    print_vec(order[1]);
    print_vec(order[2]);
    print_vec(stand(order[0], order[1], order[2]));
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
