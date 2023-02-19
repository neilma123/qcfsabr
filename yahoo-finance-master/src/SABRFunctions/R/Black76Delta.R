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
Black76Delta <- function(F0, K, Vol, tex, rfr, CallOrPut){
  if (CallOrPut %in% c('c', 'p')){
    a <- ifelse(CallOrPut == 'c', 0, -1)
  }
  else stop('CallOrPut flag can only take values c or p!')

  d1 <- (log(F0 / K) + 0.5 * Vol ^ 2  * tex) / (Vol * sqrt(tex))

  Delta <- exp(-rfr * tex) * (stats::pnorm(d1) + a)

  return(Delta)
}
