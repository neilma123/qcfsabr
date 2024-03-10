#include "quote.hpp"
#include "time_utils.hpp"
#include "curl_utils.hpp"
#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

double func(vector<double> u)
{
    return pow(u[0] - 1, 2) + pow(u[1] + 5, 2);
}
double distance(vector<double> x, vector<double> y)
{
    return sqrt(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2));
}
vector<double> centroid(vector<double> x, vector<double> y)
{
    return {(x[0] + y[0]) / 2, (x[1] + y[1]) / 2};
}
vector<double> reflected(vector<double> w, vector<double> centroid)
{
    double distance_from_first = abs(w[0] - centroid[0]);
    double distance_from_second = abs(w[1] - centroid[1]);
    return {w[0] - 2 * distance_from_first, w[1] - 2 * distance_from_second};
}
vector<double> expanded(vector<double> w, vector<double> centroid)
{
    double distance_from_first = abs(w[0] - centroid[0]);
    double distance_from_second = abs(w[1] - centroid[1]);
    return {w[0] - 4 * distance_from_first, w[1] - 4 * distance_from_second};
}
vector<double> close_contract(vector<double> w, vector<double> centroid)
{
    double first_contract = centroid[0] + 0.5 * (w[0] - centroid[0]);
    double second_contract = centroid[1] + 0.5 * (w[1] - centroid[1]);
    return {first_contract, second_contract};
}
vector<double> far_contract(vector<double> w, vector<double> centroid)
{
    double first_contract = centroid[0] + 0.5 * (w[0] - centroid[0]);
    double second_contract = centroid[1] + 0.5 * (w[1] - centroid[1]);
    return {first_contract, second_contract};
}

vector<vector<double>> shrink(vector<vector<double>> order)
{
    double first_contract = order[0][0] + 0.5 * (order[1][0] - order[0][0]);
    double second_contract = order[0][1] + 0.5 * (order[1][1] - order[0][1]);
    order[1] = {first_contract, second_contract};
    first_contract = order[0][0] + 0.5 * (order[2][0] - order[0][0]);
    second_contract = order[0][1] + 0.5 * (order[2][1] - order[0][1]);
    order[2] = {first_contract, second_contract};
    return order;
}
vector<double> mean(vector<double> x, vector<double> y, vector<double> z)
{
    return {(x[0] + y[0] + z[0]) / 3, (x[1] + y[1] + z[1]) / 3};
}
vector<double> stand(vector<double> x, vector<double> y, vector<double> z)
{
    vector<double> average = mean(x, y, z);
    vector<double> deviation_x = {pow(x[0] - average[0], 2), pow(x[1] - average[1], 2)};
    vector<double> deviation_y = {pow(y[0] - average[0], 2), pow(y[1] - average[1], 2)};
    vector<double> deviation_z = {pow(z[0] - average[0], 2), pow(z[1] - average[1], 2)};
    vector<double> stand = {pow((deviation_x[0] + deviation_y[0] + deviation_z[0]) / 2, 0.5), pow((deviation_x[1] + deviation_y[1] + deviation_z[1]) / 2, 0.5)};
    return stand;
}
void print_vec(vector<double> x)
{

    cout << x[0] << " " << x[1] << " value is " << func(x) << endl;
}
bool operator<(vector<double> lhs, vector<double> rhs)
{
    if (func(lhs) < func(rhs))
    {
        return true;
    }
    return false;
}
bool operator==(vector<double> lhs, vector<double> rhs)
{
    if (func(lhs) == func(rhs))
    {
        return true;
    }
    return false;
}
bool operator>(vector<double> lhs, vector<double> rhs)
{
    if (func(lhs) > func(rhs))
    {
        return true;
    }
    return false;
}

std::vector<vector<double>> sort_simplex(const std::vector<double> &u, const std::vector<double> &v, const std::vector<double> &w)
{
    double value_1 = func(u);
    double value_2 = func(v);
    double value_3 = func(w);

    // Create a mapping between values and vectors
    std::vector<std::pair<double, std::vector<double>>> value_vector_pairs = {
        {value_1, u},
        {value_2, v},
        {value_3, w}};

    // Sort the pairs based on the values
    std::sort(value_vector_pairs.begin(), value_vector_pairs.end(), [](const pair<double, std::vector<double>> &lhs, const pair<double, std::vector<double>> &rhs)
              { return lhs.first < rhs.first; });

    // Extract the sorted vectors
    std::vector<double> sorted_u = value_vector_pairs[0].second;
    std::vector<double> sorted_v = value_vector_pairs[1].second;
    std::vector<double> sorted_w = value_vector_pairs[2].second;

    // Return the sorted vectors
    return {sorted_u, sorted_v, sorted_w};
}

void nelson_mead()
{
    vector<double> u = {2, 1};
    vector<double> w = {1, 2};
    vector<double> v = {3, 3};
    // while (stand(u, v, w)[0] > 0.1 && stand(u, v, w)[0] > 0.1){
    vector<vector<double>> order = {u, v, w};
    order = sort_simplex(u, v, w);
    vector<double> std = {0.000001, 0.000001};
    // print_vec(stand(order[0], order[1], order[2]));
    cout << "very beginning order" << endl;
    print_vec(order[0]);
    print_vec(order[1]);
    print_vec(order[2]);
    int count = 0;
    while ((stand(order[0], order[1], order[2])[0] > std[0] || stand(order[0], order[1], order[2])[1] > std[1]))
    {
        cout << endl
             << endl;
        cout << "loop number " << count << endl;
        count++;
        order = sort_simplex(order[0], order[1], order[2]);
        vector<double> centroid_1 = centroid(order[0], order[1]);
        cout << "centroid is ";
        print_vec(centroid_1);
        vector<double> reflected_point = reflected(order[2], centroid_1);
        vector<double> expanded_point = expanded(order[2], centroid_1);
        vector<double> far_contracted_point = far_contract(order[2], centroid_1);
        vector<double> close_contracted_point = close_contract(reflected_point, centroid_1);
        cout << "reflected is ";
        print_vec(reflected_point);
        cout << "expanded is ";
        print_vec(expanded_point);
        cout << "far contracted is ";
        print_vec(far_contracted_point);
        cout << "close contracted is ";
        print_vec(close_contracted_point);
        cout << "starting here " << endl;
        cout << "beginning loop order" << endl;
        print_vec(order[0]);
        print_vec(order[1]);
        print_vec(order[2]);
        cout << "reflected point is ";
        print_vec(reflected_point);
        bool first_condition = func(reflected_point) < func(order[1]) && func(reflected_point) >= func(order[0]);
        bool second_condition = func(reflected_point) < func(order[0]);
        bool third_condition = func(expanded_point) < func(reflected_point);
        bool fourth_condition = func(reflected_point) < func(order[2]);
        bool fifth_condition = func(far_contracted_point) <= func(reflected_point);
        if (first_condition)
        {
            cout << "first hit" << endl;
        }
        if (second_condition)
        {
            cout << "second hit" << endl;
        }
        if (third_condition)
        {
            cout << "third hit" << endl;
        }
        if (fourth_condition)
        {
            cout << "fourth hit" << endl;
        }
        if (fifth_condition)
        {
            cout << "fifth hit" << endl;
        }
        else
        {
            cout << "shriking probs" << endl;
        }
        if (first_condition)
        {
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
        if (second_condition)
        {

            cout << "expanded point is ";
            print_vec(expanded_point);
            if (third_condition)
            {
                order[2] = expanded_point;
                cout << "expanded";
                print_vec(order[2]);
                cout << "to: ";
                print_vec(expanded_point);
                cout << endl;
                continue;
            }
            else
            {
                cout << "we are here alksjflskd" << endl;
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
        if (fourth_condition)
        {
            // vector<double> far_contracted_point_1 = far_contract(order[0], order[1]);
            // vector<double> far_contracted_point_2 = far_contract(order[0], order[2]);

            cout << "far contracted point is ";
            print_vec(far_contracted_point);
            if (fifth_condition){
                order[2] = far_contracted_point;
                cout << "far contracted ";
                print_vec(order[2]);
                cout << "to: ";
                print_vec(far_contracted_point);
                cout << endl;
                continue;
            }
            else{

                cout << "made it here close contracting";
                if (close_contracted_point < order[2])
                {
                    order[2] = close_contracted_point;
                    continue;
                }
                // print_vec(order[0]);
                // cout << endl;
                // print_vec(order[1]);
                // cout << endl;
                // print_vec(order[2]);
                // cout << endl;
            }
        }
        cout << "shrinking now" << endl;
        order = shrink(order);
        cout << "standard deviation is ";
        print_vec(stand(order[0], order[1], order[2]));
        cout << "order is " << endl;
        print_vec(order[0]);
        print_vec(order[1]);
        print_vec(order[2]);
        order = sort_simplex(order[0], order[1], order[2]);
    }
    print_vec(order[0]);
    print_vec(order[1]);
    print_vec(order[2]);
    print_vec(stand(order[0], order[1], order[2]));
}

int main()
{
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
