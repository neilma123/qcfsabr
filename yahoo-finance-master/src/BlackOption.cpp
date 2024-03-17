#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <BlackOption.h>

using namespace std;

    // Black76OptionPrice
double Black76OptionPriceC(double F0, double K, double Vol, double tex, double rfr, const char * CallOrPut) {

int a = strcmp(CallOrPut,"c");
if (a == 0){
    a = 1;
  } else if (strcmp(CallOrPut, "p") == 0){
    a = -1;
  } else {
    printf("CallOrPut flag can only take values c or p!");
    return 0;
    }
double d1 = (log(F0 / K) + 0.5 * pow(Vol,2)  * tex) / (Vol * sqrt(tex));  //Natural logarithm used here not sure if right. Also pow(Vol,2) maybe should be pow(0.5*Vol,2)
double d2 = d1 - Vol * sqrt(tex);

double pnormD1 = 0.5*(erf((a*d1)/sqrt(2))+1); //rewrote the normal CDF in terms of erf
double pnormD2 = 0.5*(erf((a*d2)/sqrt(2))+1);

double Price = exp(-1 * rfr * tex) * a * (F0 * pnormD1 - K * pnormD2);

return(Price);

}
    
    // Black76Delta
double Black76Delta(double F0, double K, double Vol, double tex, double rfr, const char * CallOrPut){
 
 
int a = 0;
if (strcmp(CallOrPut, "c") == 0){
    int a = 0;
  } else if (strcmp(CallOrPut, "p") == 0){
    int a = -1;
  } else {
    printf("CallOrPut flag can only take values c or p!");
    return 0;
    }

double d1 = (log(F0 / K) + 0.5 * pow(Vol,2)  * tex) / (Vol * sqrt(tex));

double pnormD1 = 0.5*(erf((d1)/sqrt(2))+1); //rewrote the normal CDF in terms of erf

double Delta = exp(-rfr * tex) * (pnormD1 + a);

return(Delta);
}

    // Black76Gamma
double BlackOption::Black76Gamma() {};

double BlackOption::Black76Vega(double SABRImpVol) {};

void BlackOption::set_imp_vol(double iv){
    this->imp_vol = iv;
}

void BlackOption::set_strike_price(double strike_in){
    this->strike = strike_in;
}
    // 
