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
    vector<double> coefficients = AlphaCubic();
    vector<double> output;
    for (auto& it: x) {
        double curr_val = coefficients[0]*pow(it, 3) + coefficients[1]*pow(it, 2) + coefficients[2]*pow(it, 1) + coefficients[0];
        output.push_back(curr_val);
        

    }
};

vector<double> SABR::ATMCalib(double guess_rho, double guess_nu) {};

vector<double> SABR::AlphaCubic() {
    double A3 = (pow(1 - this->beta, 2) * this->tex) / (24 * pow(this->fwd_rate, 2 - 2 * this->beta));

};

double SABR::SABRDelta() {};

vector<double> SABR::fullcalib(double guess_alpha, double guess_rho, double guess_nu) {};

vector<double> SABR::Paramlinearbump(string bump_param, double bump_size, bool bump_dir) {};

double SABR::SABR_to_Black76() {};

double SABR::SABR_vanna() {};

double SABR::SABR_vega() {};

double SABR::SABR_volga() {};

double SABR::SABRGamma() {};

vector<double> SABR::VolsFromATMCalib(double guess_rho, double guess_nu) {};


vector<double> VolsFromFullCalib() {};