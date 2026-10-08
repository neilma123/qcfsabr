# QuantEdge SABR statistical arbitrage

This revives the unfinished 2023–2024 project as a dependency-free C++17 research executable. It implements Hagan's lognormal SABR approximation, Black-76 pricing and Greeks, constrained SABR calibration, a real-data relative-value backtest, and Yahoo Finance history retrieval for both underlyings and explicitly named OCC option contracts.

The historical source snapshots remain available in the original local archive, but the published repository contains only the cleaned, working implementation in `src/`. The earlier prototype had incompatible SABR declarations, empty pricing/calibration functions, and a non-updating Nelder-Mead loop; those issues are resolved here.

## Run it on Windows

PowerShell:

```powershell
.\build.ps1
.\build\statarb.exe self-test
.\run_backtest.ps1
```

`run_backtest.ps1` uses `data/options_sample_2022H2.zip` when present. If absent, it downloads the free July–December 2022 sample from HistoricalData.net and extracts it. Results are written to `results/backtest_trades.csv`.

## Backtest definition

For each trading day, the program:

1. Selects the liquid European SPXW call expiration nearest 35 calendar days (allowed range 21–60 days) and infers its forward level from same-strike call/put parity.
2. Fits alpha, rho, and nu with beta fixed at 1.0 against the full implied-volatility smile using Nelder-Mead in transformed parameter space.
3. Buys the option whose market IV is most below the fitted smile and shorts the option most above it, requiring both deviations to exceed one cross-sectional residual standard deviation.
4. Vega-matches the short leg, delta-hedges the pair at entry, and exits both options at the next day's close.
5. Reports an optimistic midpoint result and a conservative result that buys at ask/sells at bid at both entry and exit. The hedge is assumed executable without slippage; commissions and margin financing are excluded.

This is an exploratory six-month backtest, not evidence of a deployable strategy. End-of-day quotes across strikes are not synchronized, the same surface is used to fit and rank residuals, and results may be sensitive to filters, hedge conventions, and the sample period.

## Yahoo contract history

The original project used Yahoo's retired authenticated CSV endpoint. The new adapter uses the public chart endpoint and accepts the exact option symbol the old code anticipated:

```powershell
.\build\statarb.exe yahoo SPY 2026-01-02 2026-02-01
.\build\statarb.exe yahoo SPY261218C00600000 2026-01-02 2026-02-01
```

Yahoo may return no history for expired/delisted contracts. It also does not provide a complete point-in-time historical option chain, so it is suitable for individual contracts and ongoing collection, not for reconstructing a survivorship-safe backtest. The archived daily chain sample is used for that purpose.

## Portable CMake build

If CMake is installed:

```text
cmake -S . -B build
cmake --build build --config Release
```

Data source: HistoricalData.net free options sample, July–December 2022. See `data/README.md` and `data/LICENSE.txt` for schema, verification, and license details.
