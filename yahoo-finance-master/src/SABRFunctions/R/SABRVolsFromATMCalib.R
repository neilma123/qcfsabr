#' ATM Calibration Wrapper Function for SABR Parameters
#'
#' Runs the complete calibration against market volatilities and strikes by calling the ATM
#' calibration method, SABRATMCALIB.
#'
#' @param F0 Current forward rate
#' @param ATMVol Current lognormal ATM volatility corresponding to F0 in the market
#' @param Strikes VECTOR of strike prices
#' @param MarketVols VECTOR of LOGNORMAL (i.e. Black-76) market-quoted implied volatilities
#' @param tex Time to expiry of option, measured in years
#' @param Beta Shape parameter of SABR schema, EITHER evaluated using historical data OR preset by user
#' @param guess_Rho Initial user-defined guess of Rho value, MUST be bounded between -1 and 1
#' @param guess_Nu Initial user-defined guess of Nu value, MUST be non-zero
#'
#' @return A list object containing each of the 4 calibrated parameters, plus a vector of
#' the input strikes, and a vector of the calibrated Black-76-equivalent volatilities.
#' @export
#'
#' @examples
#' data(sabrcalibdata)
#' calibrun <- sabrcalibdata[sabrcalibdata$Point == '3M10Y',]
#' SABRVolsFromATMCalib(F0 = 0.0266, ATMVol = 0.4084, Strikes = calibrun$Strike,
#' MarketVols = calibrun$BlackVol, tex = 0.25, Beta = 0.5, guess_Rho = 0.05, guess_Nu = 0.7)
#'
SABRVolsFromATMCalib <- function(F0, ATMVol, Strikes, MarketVols, tex, Beta, guess_Rho, guess_Nu){
   #Step 1: Run the calibration for SABR Rho and Nu:
  CalibratedRhoNuResults <- SABRFunctions::SABRATMCalib(F0, ATMVol, Strikes, MarketVols, tex, Beta, guess_Rho, guess_Nu)

  #Extract Rho and Nu from the calibration process:
  Calib_Rho <- CalibratedRhoNuResults$par[1]
  Calib_Nu <- CalibratedRhoNuResults$par[2]

  #Step 2: Calculate the calibrated SABR ATM Alpha:
  Calib_Alpha <- SABRFunctions::ATMVolToSABRAlpha(F0, ATMVol, tex, Beta, Calib_Rho, Calib_Nu)

  #Step 3: Calculate the cablibrated SABR Black-76 equivalent vols:
  n <- length(Strikes)
  Calib_SABRVols <- rep(0, n)
  for (i in 1:n){
    Calib_SABRVols[i] <- SABRFunctions::SABRtoBlack76(F0, Strikes[i], tex, Calib_Alpha, Beta, Calib_Rho, Calib_Nu)
  }

  #Step 4: Combine the results into a list of items to return:
  ResultsList <- list(SABR_Alpha = Calib_Alpha, SABR_Beta = Beta, SABR_Rho = Calib_Rho, SABR_Nu = Calib_Nu, SABR_Strikes = Strikes,
                      SABR_Vols = Calib_SABRVols)

  return(ResultsList)
}
