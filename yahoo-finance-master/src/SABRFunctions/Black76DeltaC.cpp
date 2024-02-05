/*
#' Black-76 Closed Form Delta
#'
#' Closed-form solution for the Delta of a European call or put on a forward contract or rate,
#' using the Black-76 formulation. For swaptions, provides the simple Delta for a European-style
#' exercise payer or receiver swaption.
#'
#' @param F0 Current forward rate
#' @param K Strike of the option
#' @param Vol Implied volatility, MUST BE IN LOGNORMAL TERMS, can be taken from SABRTOBLACK76
#' @param tex Time to expiry, in years, of the option
#' @param rfr Riskless rate, best taken as either the 10y or 30y government zero rate
#' @param CallOrPut Takes values of 'c' or 'p', and nothing else - determines whether you are pricing a call or put
#'
#' @return Standard Black-76 Delta of a European option on a forward contract
#' @export
#'
#' @examples Black76Delta(F0 = 0.0266, K = 0.0250, Vol = 0.4084, tex = 0.25, rfr = 0.02, CallOrPut = 'c')

*/

#include <cmath>
#include <iostream>
#include <string.h>
#include "BlackFuncts.h"

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

//As of 10/30/23 this works (matches with output in RStudio)