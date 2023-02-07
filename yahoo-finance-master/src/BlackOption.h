#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

class BlackOption {
private:
    // add variables
    double fwd_rate;
    double strike;
    double imp_vol;
    double expiry;
    double rfr = 0.458; //  hardcoded for now, check later
    bool cp; // True if Call, False if Put
public:
    
    // Non-default constructor
    BlackOption(double fwd_rate, double strike, double imp_vol, double expiry, double rfr):
                fwd_rate{fwd_rate}, strike{strike}, imp_vol{imp_vol}, expiry{expiry}, rfr{rfr} {};

    // Black76OptionPrice
    double Black76OptionPrice();
    
    // Black76Delta
    double Black76Delta();

    // Black76Gamma
    double Black76Gamma();

    double Black76Vega();

    // 
};