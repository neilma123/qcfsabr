% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/data.R
\docType{data}
\name{sabrcalibdata}
\alias{sabrcalibdata}
\title{Calibration data for SABRFunctions package}
\format{
A data frame with 56 rows of 3 variables:
\describe{
\item{Point}{Name of forward rate, used for filtering}
\item{Strike}{Strike rate of various options traded in the market}
\item{BlackVol}{Implied lognormal volatility of option traded at that strike}
}
}
\source{
{Created using Bloomberg BVOL data from May 23, 2022}
}
\usage{
sabrcalibdata
}
\description{
Bloomberg implied volatility smile data by strike from May 23, 2022
}
\examples{
data('sabrcalibdata')  ###Lazy loading
}
\keyword{datasets}
