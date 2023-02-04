#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <BlackOption.h>
#include <vector>

using namespace std;

class SABR {
private:
    vector<BlackOption> options;
    double fwd_rate;
    double ATM_vol;
    double tex;
    double beta;
    double rho;
    double nu;
    vector<double> strikes;
    vector<double> market_vols;
    double rfr = 0.458; //  hardcoded for now, check later
    bool cp; // TRUE if Call option, FALSE if put option
public:

    double ATMVol_to_Sabr_alpha();

    vector<double> SABRATMCalib(double guess_rho, double guess_nu);

    string SABRAlphaCubic(double curr_val);

    double SABRDelta();

    vector<double> SABRfullcalib();

    vector<double> SABRParamlinearbump();

    double SABR_to_Black76();

    double SABR_vanna();

    double SABR_vega();

    double SABR_volga();

    double SABRGamma();

    vector<double> SABRVolsFromATMCalib();
    
    vector<double> SABRVolsFromFullCalib();
};
