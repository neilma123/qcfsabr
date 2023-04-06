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

vector<double> SABR::AlphaCubic() {
    double A3 = (pow(1 - this->beta, 2) * this->tex) / (24 * pow(this->fwd_rate, 2 - 2 * this->beta));
    double A2 = (this->rho * this->nu * this ->beta * this-> tex) / (4 * pow(this->fwd_rate, 1 - this->beta));
    double A1 = 1 + (2 - 3 * pow(this->rho, 2)) / 24 * pow(this->nu, 2) * this->tex;
    double A0 = -1 * this->ATM_vol * pow(this->fwd_rate, 1 - this->beta);
    vector<double> coefficients = {A3, A2, A1, A0};
    coefficients.resize(4);
    return coefficients;
};

// Received an input Black Option object
double SABR::SABRDelta(BlackOption blk) {

    double SABR_imp_vol = SABR_to_Black76();
    blk.imp_vol = SABR_imp_vol;
    double SABR_black_76_delta = blk.Black76Delta();



}; // Kapil

vector<double> SABR::fullcalib(double guess_alpha, double guess_rho, double guess_nu) {};

vector<double> SABR::Paramlinearbump(string bump_param, double bump_size, bool bump_dir) {};

double SABR::SABR_to_Black76(double fwd_rate, double strike, double alpha, double tex) {
    double k1 = pow(fwd_rate * strike, (1 - this->beta) / 2);
    double k2 = 1 + pow(1 - this->beta, 2) / 24 * pow(log(fwd_rate / strike), 2) + pow(1 - this->beta, 4) / 1920 * pow(log(fwd_rate / strike), 4);
    double z = this->nu / alpha * pow(fwd_rate * strike, (1 - this->beta) / 2) * log(fwd_rate / strike);
    double xz = log((sqrt(1 - 2 * this->rho * z + z * z) + z - this->rho) / (1 - this->rho));
    double k3 = pow(1 - this->beta, 2) / 24 * pow(alpha, 2) / (pow(fwd_rate * strike, 1 - this->beta));
    double k4 = 1 / 4 * this->rho * this->beta * this->nu * alpha / (pow(fwd_rate * strike, (1 - this->beta) / 2));
    double k5 = (2 - 3 * pow(this->rho, 2)) / 24 * pow(this->nu, 2);

    if (fwd_rate != strike) {
        return (alpha / (k1 * k2)) * (z / xz) * (1 + tex * (k3 + k4 + k5));
    }
    else {
        return alpha / (k1 * k2) * 1 * (1 + tex * (k3 + k4 + k5));
    }
};

double SABR::SABR_vanna() {};

double SABR::SABR_vega() {};

double SABR::SABR_volga() {};

double SABR::SABRGamma() {};

vector<double> SABR::VolsFromATMCalib(double guess_rho, double guess_nu) {};

vector<double> VolsFromFullCalib() {};