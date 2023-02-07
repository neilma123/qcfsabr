## ---- include = FALSE---------------------------------------------------------
knitr::opts_chunk$set(
  collapse = TRUE,
  comment = "#>"
)

## ----setup, fig.width = 8, fig.height = 6-------------------------------------
library(SABRFunctions)
library(ggplot2)
library(dplyr)
data("sabrcalibdata")
ggplot(data = sabrcalibdata, aes(x = Strike * 100, y = BlackVol * 100, colour = Point)) +
  geom_point() +
  geom_line() +
  labs(x = 'Swap Strike Rate (%)', y = 'Market-Quoted Black-76 Implied Volatility (%)',
       title = 'Market Smiles for Commonly Traded Swaption Contracts',
       subtitle = 'Implied Volatility Against Absolute Strike for Common Swap Tenors as of May 23 2022',
       caption = '(Source: Bloomberg)',
       colour = 'Forward Rate')

## ---- fig.width = 8, fig.height = 6-------------------------------------------
data('histratevoldata')
ggplot(data = histratevoldata, aes(x = log(Forward), y = log(BlackVol), colour = Rate)) +
  geom_point() +
  geom_line() +
  labs(x = 'Log of ATM Forward Rates', y = 'Log of Black ATM Vols',
       title = 'Parametrisation of SABR Beta Shape Parameters',
       subtitle = 'Relationship Between Log of Forward Rates and Log of ATM Black Vols with Linear Regression Lines',
       caption = '(Source: Bloomberg)') +
  geom_smooth(method = 'lm')

## -----------------------------------------------------------------------------
data("histratevoldata")
library(dplyr)
hist3m10y <- histratevoldata %>% filter(Rate == '3M10Y')
beta_est <- lm(log(histratevoldata$BlackVol) ~ log(histratevoldata$Forward))
summary(beta_est)
histBeta <- as.numeric(beta_est$coefficients[2] + 1)
print(histBeta)

## ---- fig.width = 8, fig.height = 6-------------------------------------------
data('curves')
library(ggplot2)
ggplot(data = curves, aes(x = Term, y = Value, colour = Type)) +
  geom_line() +
  geom_point() +
  labs(x = 'Term (Years)', y = 'Rate (%)',
       title = 'USD SOFR Swap and Zero Curves',
       subtitle = 'Fixed 30/360 vs SOFR Rates as of May 23 2022',
       caption = '(Source: Bloomberg)',
       colour = 'Rate Type')

## -----------------------------------------------------------------------------
data("sabrcalibdata")
library(dplyr)
sabr3m10y <- sabrcalibdata %>% filter(Point == '3M10Y')

#Setup SABR params
F0 <- sabr3m10y$Strike[1]          #ATM Forward Rate
K <- sabr3m10y$Strike              #Vector of strikes
MarketVols <- sabr3m10y$BlackVol   #Vector of vols

t <- 0.25                          #Assume 3M to expiry of option

#Initialise SABR parameters
Alpha <- 0.05                      #Best guess of initial Alpha
Beta <- 0.5                        #Assume simple 0.5 Beta
Rho <- 0.1                         #Guess value must be between -1 and 1
Nu <- 0.7                          #Guess value must be greater than 0

SABR3M10YFullCalib <- SABRVolsFromFullCalib(F0, K, MarketVols, t, Beta, Alpha, Rho, Nu)
SABR3M10YFullCalib

## -----------------------------------------------------------------------------
data("sabrcalibdata")
library(dplyr)
sabr3m10y <- sabrcalibdata %>% filter(Point == '3M10Y')

#Setup SABR params
F0 <- sabr3m10y$Strike[1]          #ATM Forward Rate
ATMVol <- sabr3m10y$BlackVol[1]    #ATM Black vol quoted in market
K <- sabr3m10y$Strike              #Vector of strikes
MarketVols <- sabr3m10y$BlackVol   #Vector of vols

t <- 0.25                          #Assume 3M to expiry of option

#Initialise SABR parameters
Beta <- 0.5                        #Assume simple 0.5 Beta
Rho <- 0.1                         #Guess value must be between -1 and 1
Nu <- 0.7                          #Guess value must be greater than 0

SABR3M10YATMCalib <- SABRVolsFromATMCalib(F0, ATMVol, K, MarketVols, t, Beta, Rho, Nu)
SABR3M10YATMCalib

## ---- fig.width = 8, fig.height = 6-------------------------------------------
data("sabrfitteddata")
library(ggplot2)
library(dplyr)
Fitted3M10YData <- sabrfitteddata %>% filter(Point == '3M10Y') %>% select(Method, Strike, Value)
ATM <- Fitted3M10YData$Strike[Fitted3M10YData$Method == 'MARKET'][1] * 100
ATMVOL <- Fitted3M10YData$Value[Fitted3M10YData$Method == 'MARKET'][1] * 100
ggplot(data = Fitted3M10YData, aes(x = Strike * 100, y = Value * 100, colour = Method)) +
  geom_point(aes(x = ATM, y = ATMVOL), colour = 'red', size = 5, stroke = 1, fill = 'yellow', shape = 21) +
  geom_point() +
  geom_line() +
  labs(x = 'Swaption Strike (%)', y = 'Swaption Implied Black-76 (Lognormal) Volatility (%)',
       title = 'Comparison of 3M10Y Swaption Volatilities Against Fitted Values',
       subtitle = 'ATM and Full Calibration of SABR, with Actual ATM Highlighted')

## ---- fig.width = 8, fig.height = 6-------------------------------------------
data("sabrcalibdata")
library(tidyr)
sabr3m10y <- sabrcalibdata %>% filter(Point == '3M10Y')

#Setup SABR params
F0 <- 0.0266                       #ATM Forward Rate
K <- 0.0250                        #Strike for graphing
ATMVol <- sabr3m10y$BlackVol[1]    #ATM Black vol quoted in market
MarketStrikes <- sabr3m10y$Strike  #Vector of strikes
MarketVols <- sabr3m10y$BlackVol   #Vector of vols
rfr <- 0.02                        #Riskless rate for discounting

tex <- 0.25                          #Assume 3M to expiry of option

#Initialise SABR parameters
Alpha <- 0.05                      #Best guess of initial Alpha
Beta <- 0.5                        #Assume simple 0.5 Beta
Rho <- 0.1                         #Guess value must be between -1 and 1
Nu <- 0.7                          #Guess value must be greater than 0

SABR3M10YATMCalib <- SABRVolsFromATMCalib(F0, ATMVol, MarketStrikes, MarketVols, tex, Beta, Rho, Nu)
SABR3M10YFullCalib <- SABRVolsFromFullCalib(F0, MarketStrikes, MarketVols, tex, Beta, Alpha, Rho, Nu)

#ATM calibration runs:
Alpha_ATM <- SABR3M10YATMCalib$SABR_Alpha
Beta_ATM <- SABR3M10YATMCalib$SABR_Beta
Rho_ATM <- SABR3M10YATMCalib$SABR_Rho
Nu_ATM <- SABR3M10YATMCalib$SABR_Nu

#Full calibration runs:
Alpha_Full <- SABR3M10YFullCalib$SABR_Alpha
Beta_Full <- SABR3M10YFullCalib$SABR_Beta
Rho_Full <- SABR3M10YFullCalib$SABR_Rho
Nu_Full <- SABR3M10YFullCalib$SABR_Nu

### Delta comparison:
x <- seq(0.001, 0.05, by = 0.001)
n <- length(x)

BSDeltaVec <- ATMDeltaVec <- FullDeltaVec <- rep(0, n)
for (i in 1:n){
  BSDeltaVec[i] <- Black76Delta(x[i], K, ATMVol, tex, rfr, 'c')
  ATMDeltaVec[i] <- SABRDelta(x[i], K, tex, rfr, 'c', Alpha_ATM, Beta_ATM, Rho_ATM, Nu_ATM)
  FullDeltaVec[i] <- SABRDelta(x[i], K, tex, rfr, 'c', Alpha_Full, Beta_Full, Rho_Full, Nu_Full)
}

DeltasMat <- as.data.frame(cbind(x, BSDeltaVec, ATMDeltaVec, FullDeltaVec))
names(DeltasMat) <- c('Strike', 'Black76', 'ATM', 'Full')
plotDeltaMat <- pivot_longer(DeltasMat, cols = c('Black76', 'ATM', 'Full'), names_to = 'Method', values_to = 'Delta')

###FINAL DELTA PLOT
ggplot(data = plotDeltaMat, aes(x = Strike * 100, y = Delta * 100, colour = Method)) +
  geom_point() +
  geom_line() +
  labs(x = '3M10Y Forward Rate (%)', y = 'Delta of 3M Option (%)',
       title = 'Delta of 3M10Y Option by Method',
       subtitle = 'Comparing Black-76 Delta with SABR ATM and Full Methods')


## ---- fig.width = 8, fig.height = 6-------------------------------------------
data("sabrcalibdata")
library(tidyr)
sabr3m10y <- sabrcalibdata %>% filter(Point == '3M10Y')

#Setup SABR params
F0 <- 0.0266                       #ATM Forward Rate
K <- 0.0250                        #Strike for graphing
ATMVol <- sabr3m10y$BlackVol[1]    #ATM Black vol quoted in market
MarketStrikes <- sabr3m10y$Strike  #Vector of strikes
MarketVols <- sabr3m10y$BlackVol   #Vector of vols
rfr <- 0.02                        #Riskless rate for discounting

tex <- 0.25                          #Assume 3M to expiry of option

#Initialise SABR parameters
Alpha <- 0.05                      #Best guess of initial Alpha
Beta <- 0.5                        #Assume simple 0.5 Beta
Rho <- 0.1                         #Guess value must be between -1 and 1
Nu <- 0.7                          #Guess value must be greater than 0

SABR3M10YATMCalib <- SABRVolsFromATMCalib(F0, ATMVol, MarketStrikes, MarketVols, tex, Beta, Rho, Nu)
SABR3M10YFullCalib <- SABRVolsFromFullCalib(F0, MarketStrikes, MarketVols, tex, Beta, Alpha, Rho, Nu)

#ATM calibration runs:
Alpha_ATM <- SABR3M10YATMCalib$SABR_Alpha
Beta_ATM <- SABR3M10YATMCalib$SABR_Beta
Rho_ATM <- SABR3M10YATMCalib$SABR_Rho
Nu_ATM <- SABR3M10YATMCalib$SABR_Nu

#Full calibration runs:
Alpha_Full <- SABR3M10YFullCalib$SABR_Alpha
Beta_Full <- SABR3M10YFullCalib$SABR_Beta
Rho_Full <- SABR3M10YFullCalib$SABR_Rho
Nu_Full <- SABR3M10YFullCalib$SABR_Nu

### Vega comparison:
x <- seq(0.001, 0.05, by = 0.001)
n <- length(x)

BSGammaVec <- ATMGammaVec <- FullGammaVec <- rep(0, n)
for (i in 1:n){
  BSGammaVec[i] <- Black76Gamma(x[i], K, ATMVol, tex, rfr)
  ATMGammaVec[i] <- SABRGamma(x[i], K, tex, rfr, Alpha_ATM, Beta_ATM, Rho_ATM, Nu_ATM)
  FullGammaVec[i] <- SABRGamma(x[i], K, tex, rfr, Alpha_Full, Beta_Full, Rho_Full, Nu_Full)
}

GammasMat <- as.data.frame(cbind(x, BSGammaVec, ATMGammaVec, FullGammaVec))
names(GammasMat) <- c('Strike', 'Black76', 'ATM', 'Full')
plotGammaMat <- pivot_longer(GammasMat, cols = c('Black76', 'ATM', 'Full'), names_to = 'Method', values_to = 'Gamma')

#Plot Gammas
ggplot(data = plotGammaMat, aes(x = Strike * 100, y = Gamma, colour = Method)) +
  geom_point() +
  geom_line() +
  labs(x = '3M10Y Forward Rate (%)', y = 'Gamma of 3M Option',
       title = 'Gamma of 3M10Y Option by Method',
       subtitle = 'Comparing Black-76 Gamma with SABR ATM and Full Methods')


## ---- fig.width = 8, fig.height = 6-------------------------------------------
data("sabrcalibdata")
library(tidyr)
sabr3m10y <- sabrcalibdata %>% filter(Point == '3M10Y')

#Setup SABR params
F0 <- 0.0266                       #ATM Forward Rate
K <- 0.0250                        #Strike for graphing
ATMVol <- sabr3m10y$BlackVol[1]    #ATM Black vol quoted in market
MarketStrikes <- sabr3m10y$Strike  #Vector of strikes
MarketVols <- sabr3m10y$BlackVol   #Vector of vols
rfr <- 0.02                        #Riskless rate for discounting

tex <- 0.25                          #Assume 3M to expiry of option

#Initialise SABR parameters
Alpha <- 0.05                      #Best guess of initial Alpha
Beta <- 0.5                        #Assume simple 0.5 Beta
Rho <- 0.1                         #Guess value must be between -1 and 1
Nu <- 0.7                          #Guess value must be greater than 0

SABR3M10YATMCalib <- SABRVolsFromATMCalib(F0, ATMVol, MarketStrikes, MarketVols, tex, Beta, Rho, Nu)
SABR3M10YFullCalib <- SABRVolsFromFullCalib(F0, MarketStrikes, MarketVols, tex, Beta, Alpha, Rho, Nu)

#ATM calibration runs:
Alpha_ATM <- SABR3M10YATMCalib$SABR_Alpha
Beta_ATM <- SABR3M10YATMCalib$SABR_Beta
Rho_ATM <- SABR3M10YATMCalib$SABR_Rho
Nu_ATM <- SABR3M10YATMCalib$SABR_Nu

#Full calibration runs:
Alpha_Full <- SABR3M10YFullCalib$SABR_Alpha
Beta_Full <- SABR3M10YFullCalib$SABR_Beta
Rho_Full <- SABR3M10YFullCalib$SABR_Rho
Nu_Full <- SABR3M10YFullCalib$SABR_Nu

### Vega comparison:
x <- seq(0.001, 0.05, by = 0.001)
n <- length(x)

BSVegaVec <- ATMVegaVec <- FullVegaVec <- rep(0, n)
for (i in 1:n){
  BSVegaVec[i] <- Black76Vega(x[i], K, ATMVol, tex, rfr)
  ATMVegaVec[i] <- SABRVega(x[i], K, tex, rfr, Alpha_ATM, Beta_ATM, Rho_ATM, Nu_ATM)
  FullVegaVec[i] <- SABRVega(x[i], K, tex, rfr, Alpha_Full, Beta_Full, Rho_Full, Nu_Full)
}

VegasMat <- as.data.frame(cbind(x, BSVegaVec, ATMVegaVec, FullVegaVec))
names(VegasMat) <- c('Strike', 'Black76', 'ATM', 'Full')
plotVegaMat <- pivot_longer(VegasMat, cols = c('Black76', 'ATM', 'Full'), names_to = 'Method', values_to = 'Vega')

#Plot Vegas
ggplot(data = plotVegaMat, aes(x = Strike * 100, y = Vega, colour = Method)) +
  geom_point() +
  geom_line() +
  labs(x = '3M10Y Forward Rate (%)', y = 'Vega of 3M Option',
       title = 'Vega of 3M10Y Option by Method',
       subtitle = 'Comparing Black-76 Vega with SABR ATM and Full Methods')


