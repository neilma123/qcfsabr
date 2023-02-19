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

    vector<double> ATMCalib(double guess_rho, double guess_nu);

    vector<double> AlphaCubic();

    double SABRDelta();

    vector<double> fullcalib(double guess_alpha, double guess_rho, double guess_nu);

    vector<double> Paramlinearbump(string bump_param, double bump_size, bool bump_dir);

    double SABR_to_Black76();

    double SABR_vanna();

    double SABR_vega();

    double SABR_volga();

    double SABRGamma();

    vector<double> VolsFromATMCalib(double guess_rho, double guess_nu);
    
    vector<double> VolsFromFullCalib(double guess_alpha, double guess_rho, double guess_nu);
};
