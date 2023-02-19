#' Full Calibration of SABR Parameters Against Market Vols
#'
#' Calibrates Alpha Rho and Nu such that sum of square errors between Black-76-equivalent SABR vols
#' and market observed vols are minimised. For a given Alpha, Rho, and Nu, calculates the resulting
#' Black-76 vol, then takes SSE. Minimses SSE function using non-linear optimisation.
#'
#' @param F0 Current forward rate
#' @param Strikes VECTOR of strike prices
#' @param MarketVols VECTOR of LOGNORMAL (i.e. Black-76) market-quoted implied volatilities
#' @param tex Time to expiry of option, measured in years
#' @param Beta Shape parameter of SABR schema, EITHER evaluated using historical data OR preset by user
#' @param guess_Alpha Initial user-defined guess of Alpha value, MUST be non-zero
#' @param guess_Rho Initial user-defined guess of Rho value, MUST be bounded between -1 and 1
#' @param guess_Nu Initial user-defined guess of Nu value, MUST be non-zero
#'
#' @return List of outputs from the constrOptim function that includes the parameters for calibrated
#' Alpha/Rho/Nu
#' @export
#'
#' @examples
#' data(sabrcalibdata)
#' calibrun <- sabrcalibdata[sabrcalibdata$Point == '3M10Y',]
#' SABRFullCalib(F0 = 0.0266, Strikes = calibrun$Strike, MarketVols = calibrun$BlackVol,
#' tex = 0.25, Beta = 0.5, guess_Alpha = 0.05, guess_Rho = 0.05, guess_Nu = 0.7)
SABRFullCalib <- function(F0, Strikes, MarketVols, tex, Beta, guess_Alpha, guess_Rho, guess_Nu){
  # Some basic error-checking:
  if (length(Strikes) != length(MarketVols)) stop('Strikes vector must be same length as market data!')
  if (guess_Alpha <= 0) stop('Diffusion parameter must be non-zero!')
  if (abs(guess_Rho) > 1) stop('Correlation parameter Rho must be between -1 and 1!')
  if (guess_Nu <= 0) stop('Vol-of-vol parameter must be non-zero!')

  #Create empty vector of vols for calibration:
  n <- length(Strikes)
  CalibVols <- rep(0, n)

  #Pass in the vector of initial parameter guesses:
  init_params <- c(guess_Alpha, guess_Rho, guess_Nu)

  #Define the internal sum of square errors (SSE) function:
  SSE <- function(calibparams, F0, Strikes, MarketVols, tex, Beta){
    v1 <- calibparams[1]
    v2 <- calibparams[2]
    v3 <- calibparams[3]

    for (i in 1:n){
      CalibVols[i] <- SABRFunctions::SABRtoBlack76(F0, Strikes[i], tex, v1, Beta, v2, v3)
    }

    y <- sum((CalibVols - MarketVols)^2)
    return(y)
  }

  #Define matrix and vector of constraints
  #We must setup the constraints such that:
  #alpha >= 0
  #rho >= -1
  #rho <= 1
  #nu > 0
  ui <- matrix(c(1,0,0,0,0,1,-1,0,0,0,0,1), ncol = 3)
  ci <- c(0,-1,-1,0)

  CalibSet <- stats::constrOptim(c(guess_Alpha, guess_Rho, guess_Nu), SSE, NULL, ui = ui, ci = ci, F0 = F0, Strikes = Strikes, tex = tex, Beta = Beta,
                          MarketVols = MarketVols)
  return(CalibSet)
}
