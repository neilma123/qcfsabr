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
    string cubic_1 = AlphaCubic(x[1]);
    string cubic_2 = AlphaCubic(x[2]);
    

};

vector<double> ATMCalib(double guess_rho, double guess_nu) {};

string AlphaCubic(double curr_val) {
    
    


};

double SABRDelta() {};

vector<double> fullcalib() {};

vector<double> Paramlinearbump() {};

double SABR_to_Black76() {};

double SABR_vanna() {};

double SABR_vega() {};

double SABR_volga() {};

double SABRGamma() {};

vector<double> VolsFromATMCalib() {};


vector<double> VolsFromFullCalib() {};