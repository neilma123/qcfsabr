#include <iostream>
#include <cmath>
#include <algorithm>
#include <string>
#include <BlackOption.h>
#include <vector>
#include <SABR.h>

using namespace std;

double SABR::ATMVol_to_Sabr_alpha(double tex, double fwd_rate) {
    vector<double> roots;
    // pass in the atm black option
    vector<double> coef = AlphaCubic(tex, fwd_rate);
    double x = 0;

    while (x < 100000) {
        double y = coef[0] * pow(x, 3) + coef[1] * pow(x, 2)  + coef[2] * x + coef[3];
        if (y >= -.00001 && y <= .00001){
            return x;
        }
        x += .00001;
    }
    exit;
};

SABR::SABR(vector<BlackOption> op_in, double atmvol_in, double beta_in, double rho_in, double nu_in, 
        vector<double> strikes_in, vector<double> market_vols_in, double rfr_in, bool cp_in): 
        options(op_in), ATM_vol(atmvol_in), beta(beta_in), rho(rho_in), nu(nu_in), strikes(strikes_in), market_vols(market_vols_in),
        rfr(rfr_in), cp(cp_in) {};


vector<double> SABR::ATMCalib(double guess_rho, double guess_nu, double tex, double fwd_rate) {
    if (this->strikes.size() != this->market_vols.size()) {
        cout << "Strikes vector must be same length as market data!" << endl;
        exit(1);
    }
    if (abs(guess_rho) > 1) {
        cout << "Correlation parameter Rho must be between -1 and 1!" << endl;
        exit(1);
    }
    if (guess_nu <= 0) {
        cout << "Vol-of-vol parameter must be non-zero!" << endl;
        exit(1);
    }

    vector<double> calib_vols(this->strikes.size(), 0);
    vector<double> init_params = {guess_rho, guess_nu};

    int v1 = guess_rho;
    int v2 = guess_nu;
 
    double ATMAlpha = SABR::ATMVol_to_Sabr_alpha(tex, fwd_rate);
    for (uint i = 0; i < this->strikes.size(); ++i) {
        calib_vols[i] = SABR::SABR_to_Black76(fwd_rate, this->strikes[i], this->alpha, tex);
    }
    
    double y = 0;
    for (uint i = 0; i < this->strikes.size(); ++i) {
        y += pow(calib_vols[i] - market_vols[i], 2);
    }

    return y;
    

};

vector<double> SABR::AlphaCubic(double tex, double fwd_rate) {
    double A3 = (pow(1 - this->beta, 2) * tex) / (24 * pow(fwd_rate, 2 - 2 * this->beta));
    double A2 = (this->rho * this->nu * this ->beta * tex) / (4 * pow(fwd_rate, 1 - this->beta));
    double A1 = 1 + (2 - 3 * pow(this->rho, 2)) / 24 * pow(this->nu, 2) * tex;
    double A0 = -1 * this->ATM_vol * pow(fwd_rate, 1 - this->beta);
    vector<double> coefficients = {A3, A2, A1, A0};
    coefficients.resize(4);
    return coefficients;
};

// Received an input Black Option object
double SABR::SABRDelta(BlackOption blk) {
    double SABR_imp_vol = SABR_to_Black76();
    blk.imp_vol = SABR_imp_vol;
    double SABR_black_76_delta = blk.Black76Delta();
    double SABR_black76Vega = this->SABR_vega();
    vector<double> bump_forward_up = SABRParamlinearbump();

    return 0.00;
}; // Kapil

vector<double> SABR::SABRfullcalib(double guess_alpha, double guess_rho, double guess_nu) {};

vector<double> SABR::SABRParamlinearbump(string bump_param, double bump_size, bool bump_dir) {
    
};


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

double SABR::SABR_vanna(double F0, double K, double tex, double rfr) {
    // Calculate Black-76-equivalent volatility for provided inputs:
    double SABRImpVol = SABRFunctions::SABRtoBlack76(F0, K, tex, alpha, beta, rho, nu);

    // Calculate Black-76 Vega:
    double Black76VegaPart = SABRFunctions::Black76Vega(F0, K, SABRImpVol, tex, rfr);

    // Calculate the numerical correction term:
    double SABRBumpRhoUp = SABRFunctions::SABRParamLinearBump(F0, K, tex, alpha, beta, rho, nu, "Rho", "up");
    double SABRBumpRhoDn = SABRFunctions::SABRParamLinearBump(F0, K, tex, alpha, beta, rho, nu, "Rho", "dn");

    // Calculate CENTRAL DIFFERENCE for predicted change:
    double SABRCorrectionFactor = (SABRBumpRhoUp - SABRBumpRhoDn) / 0.0001; // 0.5bp up + 0.5bp dn = 1bp total bump

    // Calculate final value:
    double FinalVanna = Black76VegaPart * SABRCorrectionFactor;

    return FinalVanna;
};

double SABR::SABR_vega(double tex, double fwd_rate, double strike) {
      double SABRImpVol = SABR::SABR_to_Black76(fwd_rate, strike, tex, this->alpha, this->beta, this->rho, this->nu);
      // CHANGE LATER
      double Black76VegaPart = this->options[0].Black76Vega(SABRImpVol);
      double SABRATMVol = SABR::SABR_to_Black76(fwd_rate, strike, tex, this->alpha, this->beta, this->rho, this->nu);
      return Black76VegaPart * SABRImpVol / SABRATMVol;
};

double SABR::SABR_volga(double fp, double strike, double tex) {
    // Calculate Black-76-equivalent volatility for provided inputs:
   double  SABRImpVol = SABR::SABR_to_Black76(fp, strike, tex, this->alpha, this->beta, this->rho, this->nu);

    // Calculate Black-76 Vega:
    double Black76VegaPart = SABR::Black76Vega(SABRImpVol);

    // Calculate the numerical correction term:
    vector<double> SABRBumpNuUp = SABR::SABRParamlinearbump(F0, K, tex, Alpha, Beta, Rho, Nu, 'Nu', bumpdir = 'up');
    vector<double> SABRBumpNuDn = SABR::SABRParamlinearbump(F0, K, tex, Alpha, Beta, Rho, Nu, 'Nu', bumpdir = 'dn');

    // Calculate CENTRAL DIFFERENCE for predicted change:
    double SABRCorrectionFactor = (SABRBumpNuUp - SABRBumpNuDn) / 0.0001; // 0.5bp up+ 0.5bp dn = 1bp total bump

    // Calculate final value:
    double FinalVolga = Black76VegaPart * SABRCorrectionFactor;

    return FinalVolga
};

double SABR::SABRGamma(double F0, double K, double tex, double rfr, double Alpha, double Beta, double Rho, double Nu) {
  // Calculate Black-76-equivalent volatility for provided inputs.
  double SABRImpVol = SABRFunctions::SABRtoBlack76(F0, K, tex, Alpha, Beta, Rho, Nu);

  // Calculate the usual d1 term.
  double d1 = (log(F0 / K) + 0.5 * SABRImpVol * SABRImpVol * tex) / (SABRImpVol * sqrt(tex));

  // Calculate Black-76 Gamma and Vega.
  double Black76GammaPart = SABRFunctions::Black76Gamma(F0, K, SABRImpVol, tex, rfr);
  double Black76VegaPart = SABRFunctions::Black76Vega(F0, K, SABRImpVol, tex, rfr);

  // Calculate the various bumps to figure out the multipliers for the Gamma equation.
  // Upward bump.
  double SABRBumpForwardUp = SABRFunctions::SABRParamLinearBump(F0, K, tex, Alpha, Beta, Rho, Nu, 'F0', "up");
  // Downward bump.
  double SABRBumpForwardDn = SABRFunctions::SABRParamLinearBump(F0, K, tex, Alpha, Beta, Rho, Nu, 'F0', "dn");

  // Calculate first-order central-differences bump.
  double Black76FirstOrderChange = (SABRBumpForwardUp - SABRBumpForwardDn) / 0.0001;

  // Calculate second-order central-differences bump.
  double Black76SecondOrderChange = (SABRBumpForwardUp - 2 * SABRImpVol + SABRBumpForwardDn) / (0.0001 * 0.0001);

  // Calculate final correction term.
  double CorrectionTerm = exp(-rfr * tex) * stats::dnorm(d1) - K * exp(-rfr * tex) * stats::dnorm(d1);

  // Put it all together to calculate Gamma.
  double FinalGamma = Black76GammaPart + Black76VegaPart * Black76SecondOrderChange + Black76FirstOrderChange * CorrectionTerm;

  return FinalGamma;
}


vector<double> SABR::SABRVolsFromATMCalib(double guess_rho, double guess_nu) {
    double calibrated_rho_nu_results = ATMCalib()

};

vector<double> SABR::SABRVolsFromFullCalib(double guess_alpha, double guess_rho, double guess_nu) {};
