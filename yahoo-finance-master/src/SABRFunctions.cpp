#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <BlackOption.h>
#include <vector>
#include <SABR.h>

using namespace std;

double SABR::ATMVol_to_Sabr_alpha() {
    vector<double> roots;
    double x[] = {-100000,100000};
    string cubic_1 = AlphaCubic(x[1]);
    string cubic_2 = AlphaCubic(x[2]);
    

};

SABR::SABR(vector<BlackOption> op_in, double atmvol_in, double beta_in, double rho_in, double nu_in, 
        vector<double> strikes_in, vector<double> market_vols_in, double rfr_in, bool cp_in): 
        options(op_in), ATM_vol(atmvol_in), beta(beta_in), rho(rho_in), nu(nu_in), strikes(strikes_in), market_vols(market_vols_in),
        rfr(rfr_in), cp(cp_in) {};

vector<double> SABR::ATMCalib(double guess_rho, double guess_nu) {};

string SABR::AlphaCubic(double curr_val) {
    
    


};

// Received an input Black Option object
double SABR::SABRDelta(BlackOption blk) {

    double SABR_imp_vol = SABR_to_Black76();
    blk.imp_vol = SABR_imp_vol;
    double SABR_black_76_delta = blk.Black76Delta();

    double SABR_black76Vega = this->SABR_vega();

    double bump_forward_up = SABRParamlinearbump()


}; // Kapil

vector<double> SABR::SABRfullcalib(double guess_alpha, double guess_rho, double guess_nu) {};

vector<double> SABR::SABRParamlinearbump(string bump_param, double bump_size, bool bump_dir) {};

double SABR::SABR_to_Black76() {};

double SABR::SABR_vanna() {};

double SABR::SABR_vega() {};

double SABR::SABR_volga() {};

double SABR::SABRGamma() {};

vector<double> SABR::SABRVolsFromATMCalib(double guess_rho, double guess_nu) {};

vector<double> SABR::SABRVolsFromFullCalib(double guess_alpha, double guess_rho, double guess_nu) {};