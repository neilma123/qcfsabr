$ErrorActionPreference = "Stop"

if (-not (Test-Path "$PSScriptRoot\build\statarb.exe")) {
    & "$PSScriptRoot\build.ps1"
}

$dataPath = "$PSScriptRoot\data\day_by_date"
if (-not (Test-Path $dataPath)) {
    $archive = "$PSScriptRoot\data\options_sample_2022H2.zip"
    if (-not (Test-Path $archive)) {
        New-Item -ItemType Directory -Force -Path "$PSScriptRoot\data" | Out-Null
        Invoke-WebRequest -Uri "https://historicaldata.net/file/options_sample_2022H2.zip" -OutFile $archive
    }
    tar -xf $archive -C "$PSScriptRoot\data"
}

New-Item -ItemType Directory -Force -Path "$PSScriptRoot\results" | Out-Null
& "$PSScriptRoot\build\statarb.exe" backtest `
    --data $dataPath `
    --underlying SPXW `
    --output "$PSScriptRoot\results\backtest_trades.csv"
if ($LASTEXITCODE -ne 0) { throw "Backtest failed." }

