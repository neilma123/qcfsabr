#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <BlackOption.h>
#include <vector>
#include <SABR.h>

using namespace std;

double ATMVol_to_Sabr_alpha() {
    vector<double> roots;
    double x[] = {-100000,100000};
    string cubic_1 = SABRAlphaCubic(x[1]);
    string cubic_2 = SABRAlphaCubic(x[2]);
    
};

vector<double> SABRATMCalib(double guess_rho, double guess_nu) {};

string SABRAlphaCubic(double curr_val) {
    
    


};

double SABRDelta() {};

vector<double> SABRfullcalib() {};

vector<double> SABRParamlinearbump() {};

double SABR_to_Black76() {};

double SABR_vanna() {};

double SABR_vega() {};

double SABR_volga() {};

double SABRGamma() {};

vector<double> SABRVolsFromATMCalib() {};


vector<double> SABRVolsFromFullCalib() {};