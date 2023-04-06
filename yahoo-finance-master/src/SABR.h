#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include "BlackOption.h"
#include <vector>

using namespace std;

class SABR {
private:
    vector<BlackOption> options;
    // double fwd_rate; specific to an option
    double ATM_vol;
    // double tex;
    double beta; // pre-setting beta
    double rho; 
    double nu; 
    vector<double> strikes;
    vector<double> market_vols;
    double rfr; //  hardcoded for now, check later
    bool cp; // TRUE if Call option, FALSE if put option
public:

    SABR(vector<BlackOption> op_in, double atmvol_in, double beta_in, double rho_in, double nu_in, 
        vector<double> strikes_in, vector<double> market_vols_in, double rfr_in, bool cp_in) {};

    double ATMVol_to_Sabr_alpha();

    vector<double> ATMCalib(double guess_rho, double guess_nu);

    vector<double> AlphaCubic(double tex, double fwd_rate);

    double SABRDelta(BlackOption blk);

    vector<double> SABRfullcalib(double guess_alpha, double guess_rho, double guess_nu);

    double SABRParamlinearbump(string bump_param, double bump_size, bool bump_dir);

    double SABR_to_Black76(double fwd_rate, double strike, double alpha, double tex);

    double SABR_vanna();

    double SABR_vega();

    double SABR_volga();

    double SABRGamma();

    vector<double> SABRVolsFromATMCalib(double guess_rho, double guess_nu);
    
    vector<double> SABRVolsFromFullCalib(double guess_alpha, double guess_rho, double guess_nu);

};
