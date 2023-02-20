#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <BlackOption.h>

using namespace std;

    // Black76OptionPrice
double BlackOption::Black76OptionPrice() {

    return 0.00;
};
    
    // Black76Delta
double BlackOption::Black76Delta() {

    return 0.00;
};

    // Black76Gamma
double BlackOption::Black76Gamma() {};

double BlackOption::Black76Vega() {};

void BlackOption::set_imp_vol(double iv){
    this->imp_vol = iv;
}

void BlackOption::set_strike_price(double strike_in){
    this->strike = strike_in;
}
    // 
