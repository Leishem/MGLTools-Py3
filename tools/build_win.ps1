# Build the wheel with MSVC (found automatically by setuptools), install it
# into a fresh venv and run the test suite. Log: build\wheel-win.log
param([string]$Python = "python", [string]$Venv = "D:\Reverse_Engineer\venv-wintest")
$ErrorActionPreference = "Continue"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root
New-Item -ItemType Directory -Force build, dist-win | Out-Null
Remove-Item dist-win\*.whl -ErrorAction SilentlyContinue
& $Python -m pip wheel . -w dist-win --no-deps -v *> build\wheel-win.log
if ($LASTEXITCODE -ne 0) { "BUILD FAILED (see build\wheel-win.log)"; exit 1 }
Get-ChildItem dist-win\*.whl | Select-Object Name, Length
if (Test-Path $Venv) { Remove-Item -Recurse -Force $Venv }
& $Python -m venv $Venv
& "$Venv\Scripts\python.exe" -m pip install -q (Get-ChildItem dist-win\*.whl).FullName pytest
Push-Location $env:TEMP
& "$Venv\Scripts\python.exe" -m pytest -q "$root\tests" -p no:cacheprovider
$rc = $LASTEXITCODE
Pop-Location
exit $rc
