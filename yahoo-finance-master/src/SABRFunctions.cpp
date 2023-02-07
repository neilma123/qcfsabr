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

vector<double> SABR::ATMCalib(double guess_rho, double guess_nu) {};

string SABR::AlphaCubic(double curr_val) {
    double A3 = (pow(1 - this->beta, 2) * this->tex) / (24 * pow(this->fwd_rate, 2 - 2 * this->beta));

};

double SABR::SABRDelta() {};

vector<double> SABR::fullcalib() {};

vector<double> SABR::Paramlinearbump() {};

double SABR::SABR_to_Black76() {};

double SABR::SABR_vanna() {};

double SABR::SABR_vega() {};

double SABR::SABR_volga() {};

double SABR::SABRGamma() {};

vector<double> SABR::VolsFromATMCalib() {};


vector<double> VolsFromFullCalib() {};