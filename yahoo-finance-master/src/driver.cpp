#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include "SABR.h"
#include "quote.hpp"
#include "time_utils.hpp"
#include "curl_utils.hpp"

using namespace std;

int main(int argc, char* argv[]){

    for(int i = 0; i < argc; ++i){
        cout << argv[i] << " ";
    } // for debugging purposes

    // command line arg structure: arg[0] = [MODE] -sabr or -help arg[1] = name of ticker
    string mode(argv[1]);
    if(mode == "-help"){
        cout << "Usage [add something about how to use]";
        exit(1);
    }

    string ticker(argv[2]); // name of the ticker to be inputted into
    int num_options = 0; // placeholder for number of options contracts retrieved
    
    vector<BlackOption> options(num_options,BlackOption(0.0,0.0,0.0,0.0,0.0));
    vector<double> strike_prices(num_options, 0.00);
    vector<double> market_vols(num_options, 0.00);
    
    for(size_t i = 0; i < options.size(); ++i){
        
        // TODO: fill in data using the yfinance library
        

        strike_prices[i] = 0.00; // TODO: add data for each option contract's strike price
        market_vols[i] = 0.00; // TODO: add data for each option contract's strike price
    }

    double atmvol = 0.00; double beta = 0.00; double rho = 0.00; 
    double nu = 0.00; double rfr = 0.00; bool cp_in = true;

    cout << "Enter attributes for SABR in order: [atmvol] [beta] [rho] [nu] [rfr] [callorput]" << " ";
    cin >> atmvol >> beta >> rho >> nu >> rfr >> cp_in;
    cout << "\n";

    SABR SABR_obj(options, atmvol, beta, rho, nu, strike_prices, market_vols, rfr, cp_in);

    // TODO: assign values from the vector of input prices returned by SABRVolsFromFullCalib
    vector<double> input_st_prices(num_options, 0.00); 
    // TODO: assign values from the vector of I.V.s returned by SABRVolsFromFullCalib
    vector<double> input_ivs(num_options, 0.00);
    
    // Calling SABRVolsFromFullCalib => returns 1. Calibrated Beta, Alpha, Rho, Nu
    //                                          2. Vector of input strike prices
    //                                          3. Vector of Black-76-Equivalent I.V.s
    // TODO: Call SABRVolsFromFullCalib the right way here and assign to above vectors the right values
    double guess_alpha = 0.00; double guess_rho = 0.00; double guess_nu = 0.00;
    SABR_obj.SABRfullcalib(guess_alpha, guess_rho, guess_nu);

    vector<double> final_prices(num_options, 0.00);

    for(size_t iv = 0; iv < input_ivs.size(); ++iv){
        // changing the IVs to the calculated ones per option 
        options[iv].set_imp_vol(input_ivs[iv]);
        options[iv].set_strike_price(input_st_prices[iv]); // not sure of this is required
        double true_price = options[iv].Black76OptionPrice();
        final_prices[iv] = true_price; // changing instead of pushing back (for time and memory complexity)
    }


    // for debugging purposes
    cout << "The final list of prices for given options contracts are: \n";
    for(size_t i = 0; i < final_prices.size(); ++i){
        cout << final_prices[i] << "\n";
    }

    return 0;

}