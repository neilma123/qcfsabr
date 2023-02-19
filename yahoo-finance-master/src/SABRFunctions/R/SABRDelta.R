#' SABR First-Order Risk with Backbone Correction
#'
#' Calculates the risk due to changes in the value of the underlying forward rate using a mixture
#' of analytical and numerical estimation methods, as outlined in 'Managing Smile Risk', p12.
#' Assumes that all parameters provided are ALREADY CALIBRATED, i.e. does not attempt to run any
#' calibrations first. Using the calibrated parameters, first calculates the Black-76-equivalent
#' volatility, then calculates the Delta risk of the option given the input parameters using
#' the analytical forms for Black-76 Delta and Vega.
#'
#' @param F0 Current forward price
#' @param K Strike of the option
#' @param tex Time to expiry, expressed in years, of the option
#' @param rfr Riskless rate, best taken as either the 10y or 30y government zero rate
#' @param CallOrPut Takes values of 'c' or 'p', and nothing else - determines whether you are pricing a call or a put
#' @param Alpha Diffusion parameter in SABR scheme, calibrated as a result of using one of two methods
#' @param Beta Shape parameter of SABR schema, EITHER evaluated using historical data OR preset by user
#' @param Rho Correlation between SABR forward and diffusion processes
#' @param Nu Vol-of-vol for SABR diffusion process
#'
#' @return Black-76-equivalent Delta of the option
#' @export
#'
#' @examples SABRDelta(F0 = 0.0266, K = 0.0250, tex = 0.25, rfr = 0.02, CallOrPut = 'c',
#' Alpha = 0.0651, Beta = 0.5, Rho = -0.0356, Nu = 1.0504)
SABRDelta <- function(F0, K, tex, rfr, CallOrPut, Alpha, Beta, Rho, Nu){
  if (CallOrPut %in% c('c', 'p')){
    a <- ifelse(CallOrPut == 'c', 0, -1)
  }
  else stop('CallOrPut flag can only take values c or p!')

  #Calculate Black-76-equivalent volatility for provided inputs:
  SABRImpVol <- SABRFunctions::SABRtoBlack76(F0, K, tex, Alpha, Beta, Rho, Nu)

  #Now calculate the Black-76 Delta component of the actual SABR Delta:
  SABRBlack76Delta <- SABRFunctions::Black76Delta(F0, K, SABRImpVol, tex, rfr, CallOrPut)

  #Now calculate the Black-76 Vega component of the actual SABR Delta:
  SABRBlack76Vega <- SABRFunctions::Black76Vega(F0, K, SABRImpVol, tex, rfr)

  #Now calculate the numerical correction term:
  SABRBumpForwardUp <- SABRFunctions::SABRParamLinearBump(F0, K, tex, Alpha, Beta, Rho, Nu, 'F0', bumpdir = 'up')
  SABRBumpForwardDn <- SABRFunctions::SABRParamLinearBump(F0, K, tex, Alpha, Beta, Rho, Nu, 'F0', bumpdir = 'dn')

  #Calculate CENTRAL DIFFERENCE for predicted change:
  SABRCorrectionFactor <- (SABRBumpForwardUp - SABRBumpForwardDn) / 0.0001 #0.5bp up + 0.5bp dn = 1bp total bump

  #Add up all relevant terms:
  FinalDelta <- SABRBlack76Delta + SABRBlack76Vega * SABRCorrectionFactor

  return(FinalDelta)
}
