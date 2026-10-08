$ErrorActionPreference = "Stop"

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw "Visual Studio Build Tools were not found." }
$installation = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $installation) { throw "The Visual Studio C++ build tools are not installed." }
$developerShell = Join-Path $installation "Common7\Tools\VsDevCmd.bat"

New-Item -ItemType Directory -Force -Path "$PSScriptRoot\build" | Out-Null
$compileCommand = "call `"$developerShell`" -arch=x64 -host_arch=x64 >nul && " +
    "cl.exe /nologo /std:c++17 /EHsc /O2 /W4 " +
    "`"$PSScriptRoot\src\main.cpp`" `"$PSScriptRoot\src\sabr.cpp`" " +
    "`"$PSScriptRoot\src\backtest.cpp`" `"$PSScriptRoot\src\yahoo.cpp`" " +
    "/Fe:`"$PSScriptRoot\build\statarb.exe`""
Push-Location "$PSScriptRoot\build"
try {
    & cmd.exe /d /c $compileCommand
    $compileExit = $LASTEXITCODE
} finally {
    Pop-Location
}
if ($compileExit -ne 0) { throw "C++ build failed." }
Write-Host "Built $PSScriptRoot\build\statarb.exe"
