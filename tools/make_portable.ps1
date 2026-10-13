# Build a self-contained, zero-install portable Windows distribution of MGLTools 1.5.6 (Py3)
param(
    [string]$Venv = "D:\Reverse_Engineer\venv-wintest",
    [string]$OutputDir = "D:\Reverse_Engineer\MGLTools-1.5.6-Py3-Win64-Portable",
    [string]$ZipFile = "D:\Reverse_Engineer\MGLTools-1.5.6-Py3-Win64-Portable.zip"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot

Write-Host "=== Building Portable MGLTools 1.5.6 (Python 3) ===" -ForegroundColor Cyan

# 1. Discover base Python directory
$pyExe = "$Venv\Scripts\python.exe"
if (-not (Test-Path $pyExe)) {
    throw "Virtualenv python not found at $pyExe"
}
$basePython = (& $pyExe -c "import sys; print(sys.base_prefix)").Trim()
Write-Host "Base Python: $basePython" -ForegroundColor Green

# 2. Prepare Output Directory
if (Test-Path $OutputDir) {
    Write-Host "Removing existing $OutputDir..."
    Remove-Item -Recurse -Force $OutputDir
}
New-Item -ItemType Directory -Force "$OutputDir\python" | Out-Null

# 3. Copy Python Core Binaries & DLLs
Write-Host "Copying Python runtime binaries..."
$binFiles = @("python.exe", "pythonw.exe", "python3.dll", "python313.dll", "vcruntime140.dll", "vcruntime140_1.dll")
foreach ($f in $binFiles) {
    $src = Join-Path $basePython $f
    if (Test-Path $src) {
        Copy-Item -Path $src -Destination "$OutputDir\python"
    }
}

Write-Host "Copying DLLs and tcl..."
Copy-Item -Path (Join-Path $basePython "DLLs") -Destination "$OutputDir\python\DLLs" -Recurse
Copy-Item -Path (Join-Path $basePython "tcl") -Destination "$OutputDir\python\tcl" -Recurse

Write-Host "Copying standard library..."
New-Item -ItemType Directory -Force "$OutputDir\python\Lib" | Out-Null
robocopy (Join-Path $basePython "Lib") "$OutputDir\python\Lib" /E /XD "test" /XF "*.pyc" /NFL /NDL /NJH /NJS /NC /NS /NP | Out-Null

# 4. Copy site-packages from venv
Write-Host "Copying site-packages (MGLTools and dependencies)..."
New-Item -ItemType Directory -Force "$OutputDir\python\Lib\site-packages" | Out-Null
$spDir = "$Venv\Lib\site-packages"
robocopy $spDir "$OutputDir\python\Lib\site-packages" /E /XD "__pycache__" /XF "*.pyc" /NFL /NDL /NJH /NJS /NC /NS /NP | Out-Null

# 5. Compile Native GUI Launchers with MSVC
Write-Host "Compiling native GUI launchers (adt.exe, pmv.exe, vision.exe)..."
$vcvars = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path $vcvars)) {
    throw "MSVC vcvars64.bat not found at $vcvars"
}
$launcherC = "$root\tools\launcher.c"

$apps = @(
    @{ Name = "adt"; Title = "AutoDockTools" },
    @{ Name = "pmv"; Title = "PMV" },
    @{ Name = "vision"; Title = "Vision" }
)

foreach ($app in $apps) {
    $name = $app.Name
    $title = $app.Title
    $outExe = "$OutputDir\$name.exe"
    $cmd = "call `"$vcvars`" && cl /nologo /O2 /W3 /DLAUNCH_CMD=\`"$name\`" /DLAUNCH_TITLE=\`"$title\`" /Fe:`"$outExe`" `"$launcherC`" /link /SUBSYSTEM:WINDOWS user32.lib"
    cmd /c $cmd | Out-Null
    if (-not (Test-Path $outExe)) {
        throw "Failed to compile $outExe"
    }
    Remove-Item "$OutputDir\$name.obj" -ErrorAction SilentlyContinue
    Write-Host "Compiled $name.exe" -ForegroundColor Green
}

# 6. Create Companion Batch Launchers (for CLI/debugging)
Write-Host "Generating batch launchers..."
foreach ($app in $apps) {
    $name = $app.Name
    $batContent = @"
@echo off
setlocal
set "DIR=%~dp0"
"%DIR%python\python.exe" -m mgltools.launch $name %*
"@
    Set-Content -Path "$OutputDir\$name.bat" -Value $batContent -Encoding Ascii
}

# 7. Add ADT Utilities Launcher Batch
$adtUtilBat = @"
@echo off
setlocal
set "DIR=%~dp0"
if "%~1"=="" (
    echo Usage: adt-util.bat [script_name] [arguments]
    echo Example: adt-util.bat prepare_receptor4.py -r hsg1.pdb -o hsg1.pdbqt
    echo Available scripts in AutoDockTools\Utilities24:
    dir /b "%DIR%python\Lib\site-packages\AutoDockTools\Utilities24\*.py"
    exit /b 1
)
"%DIR%python\python.exe" "%DIR%python\Lib\site-packages\AutoDockTools\Utilities24\%~1" %2 %3 %4 %5 %6 %7 %8 %9
"@
Set-Content -Path "$OutputDir\adt-util.bat" -Value $adtUtilBat -Encoding Ascii

# 8. Copy Licenses and Add README
Copy-Item "$root\LICENSES" "$OutputDir\LICENSES.txt"

$readmeContent = @"
================================================================================
 MGLTools 1.5.6 (Python 3 Port) - Portable Windows Distribution
================================================================================

This package is completely standalone and portable.
NO Python installation or setup is required. Runs from any folder or USB drive.

Works on:
- AMD 64-bit processors (AMD Ryzen, Threadripper, EPYC)
- Intel 64-bit processors (Core i3/i5/i7/i9, Xeon)
- Qualcomm Snapdragon Windows on ARM (via Windows 11 Prism emulation)

--------------------------------------------------------------------------------
HOW TO RUN:
--------------------------------------------------------------------------------

1. AutoDockTools (ADT) GUI:
   - Double-click `adt.exe`
   - Or run `adt.bat` from terminal

2. Python Molecular Viewer (PMV) GUI:
   - Double-click `pmv.exe`
   - Or run `pmv.bat` from terminal

3. Vision Network Editor:
   - Double-click `vision.exe`
   - Or run `vision.bat` from terminal

4. Command-Line Preparation Scripts (prepare_receptor4, prepare_ligand4, etc.):
   - Open a command prompt in this folder and run:
     adt-util.bat prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
     adt-util.bat prepare_ligand4.py -l ligand.pdb -o ligand.pdbqt
     adt-util.bat prepare_gpf4.py -l ligand.pdbqt -r receptor.pdbqt -o grid.gpf
     adt-util.bat prepare_dpf42.py -l ligand.pdbqt -r receptor.pdbqt -o dock.dpf

--------------------------------------------------------------------------------
LICENSE:
--------------------------------------------------------------------------------
MGLTools is free for non-commercial educational and academic research use.
See LICENSES.txt for full copyright and licensing terms.
"@
Set-Content -Path "$OutputDir\README.txt" -Value $readmeContent -Encoding Utf8

# 9. Verify Portable Python Execution
Write-Host "Verifying portable Python runtime..."
$verifyCmd = "& `"$OutputDir\python\python.exe`" -c `"import AutoDockTools, Pmv, DejaVu, MolKit, mslib; print('PORTABLE_VERIFY_SUCCESS')`""
$res = Invoke-Expression $verifyCmd
if ($res -notmatch "PORTABLE_VERIFY_SUCCESS") {
    throw "Portable verification failed: $res"
}
Write-Host "Portable runtime verified successfully!" -ForegroundColor Green

# 10. Create Zip Archive
Write-Host "Creating zip archive: $ZipFile..."
if (Test-Path $ZipFile) {
    Remove-Item $ZipFile -Force
}
Compress-Archive -Path "$OutputDir\*" -DestinationPath $ZipFile -CompressionLevel Optimal

$zipItem = Get-Item $ZipFile
$sizeMB = [math]::Round($zipItem.Length / 1MB, 2)
Write-Host "=== Portable Package Successfully Built! ===" -ForegroundColor Cyan
Write-Host "Folder: $OutputDir"
Write-Host "Zip Archive: $ZipFile ($sizeMB MB)"
