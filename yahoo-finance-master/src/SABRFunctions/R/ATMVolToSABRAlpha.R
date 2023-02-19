#' ATM Vol to SABR Alpha calibration
#'
#' Returns the ATM SABR Alpha corresponding to the ATM Black market vol, using root solving to get the smallest positive root from SABRALPHACUBIC
#'
#' @param F0 Current forward rate
#' @param ATMVol Current BLACK ATM vol in the market corresponding to F0
#' @param tex Time to expiry of option, measured in years
#' @param Beta Shape parameter of SABR schema, EITHER evaluated using historical data OR preset by user
#' @param Rho Correlation between SABR forward and diffusion processes
#' @param Nu Vol-of-vol for SABR diffusion process
#'
#' @return Single numeric value correspoding to the calibrated ATM SABR Alpha
#' @export
#' @examples
#' ### Simple example involving a 3M option on the 10Y swap rate with 50% Beta assumed
#' ATMVolToSABRAlpha(F0 = 0.0266, ATMVol = 0.4084, tex = 0.25, Beta = 0.5, Rho = -0.0356,
#'  Nu = 1.0504)
#'
ATMVolToSABRAlpha <- function(F0, ATMVol, tex, Beta, Rho, Nu){
  roots <- rootSolve::uniroot.all(SABRFunctions::SABRAlphaCubic, c(-100000,100000), F0 = F0, ATMVol = ATMVol, tex = tex, Beta = Beta, Rho = Rho, Nu = Nu)
  alpha <- min(roots[roots>0])
  return(alpha)
}
