#include <cmath>
#include <iostream>
#include <string.h>
#include "BlackFuncts.h"
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

//Matches with RStudio output as of 10/30/23
//https://stats.stackexchange.com/a/187909