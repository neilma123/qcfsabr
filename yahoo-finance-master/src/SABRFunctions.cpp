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
    
    
};

vector<double> SABR::ATMCalib(double guess_rho, double guess_nu) {};

string SABR::AlphaCubic(double curr_val) {
    double A3 = (pow(1 - this->beta, 2) * this->tex) / (24 * pow(this->fwd_rate, 2 - 2 * this->beta));

};

// Received an input Black Option object
double SABR::SABRDelta(BlackOption blk) {

    double SABR_imp_vol = SABR_to_Black76();
    blk.imp_vol = SABR_imp_vol;
    double SABR_black_76_delta = blk.Black76Delta();



}; // Kapil

vector<double> SABR::fullcalib(double guess_alpha, double guess_rho, double guess_nu) {};

vector<double> SABR::Paramlinearbump(string bump_param, double bump_size, bool bump_dir) {};

double SABR::SABR_to_Black76() {};

double SABR::SABR_vanna() {};

double SABR::SABR_vega() {};

double SABR::SABR_volga() {};

double SABR::SABRGamma() {};

vector<double> SABR::VolsFromATMCalib(double guess_rho, double guess_nu) {};


vector<double> VolsFromFullCalib() {};