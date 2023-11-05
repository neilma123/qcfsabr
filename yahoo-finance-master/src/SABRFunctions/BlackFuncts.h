#ifndef BlackFuncts    // To make sure you don't declare the function more than once by including the header multiple times.
#define BlackFuncts

#include <cmath>
#include <iostream>
#include <string.h>

double Black76OptionPriceC(double F0, double K, double Vol, double tex, double rfr, const char * CallOrPut);
double Black76Delta(double F0, double K, double Vol, double tex, double rfr, const char * CallOrPut);
#endif