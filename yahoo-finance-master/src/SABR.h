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

    string AlphaCubic(double curr_val);

    double SABRDelta();

<<<<<<< HEAD:SABR.h
    vector<double> SABRfullcalib(double guess_alpha, double guess_rho, double guess_nu);

    vector<double> SABRParamlinearbump(string bump_param, double bump_size, bool bump_dir);
=======
    vector<double> fullcalib();

    vector<double> Paramlinearbump();
>>>>>>> 915e0e5fc8ce034e349e0e87db33d8769d017b35:yahoo-finance-master/src/SABR.h

    double SABR_to_Black76();

    double SABR_vanna();

    double SABR_vega();

    double SABR_volga();

    double SABRGamma();

<<<<<<< HEAD:SABR.h
    vector<double> SABRVolsFromATMCalib(double guess_rho, double guess_nu);
    
    vector<double> SABRVolsFromFullCalib(double guess_alpha, double guess_rho, double guess_nu);
=======
    vector<double> VolsFromATMCalib();
    
    vector<double> VolsFromFullCalib();
>>>>>>> 915e0e5fc8ce034e349e0e87db33d8769d017b35:yahoo-finance-master/src/SABR.h
};
