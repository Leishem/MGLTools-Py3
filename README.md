# MGLTools 1.5.6 for Python 3 — AutoDockTools GUI, portable for Windows, Linux and macOS

**AutoDockTools (ADT)**, the **Python Molecular Viewer (PMV)** and **Vision** from
MGLTools 1.5.6, ported from Python 2.5 / Numeric / Togl to **Python 3 /
NumPy 2 / modern OpenGL**.

The Windows release is **portable**: no installation, no Python needed.
Download, unzip, double-click `adt.exe`.
Linux x86_64 and macOS (Apple Silicon) packages are also available — see
[Download and run (Linux)](#download-and-run-linux) and
[Download and run (macOS)](#download-and-run-macos).

> ### ⬇️ [Download for Windows (MGLTools-1.5.6-Py3-Win64-Portable.zip, ~110 MB)](https://github.com/Leishem/MGLTools-Py3/releases/latest/download/MGLTools-1.5.6-Py3-Win64-Portable.zip)
>
> **Do not use the green `<> Code` → *Download ZIP* button.** That gives you the
> source code only — it has **no `adt.exe`**. The ready-to-run program is the
> zip file above (also listed under **Releases** on the right side of this page).
>
> ### ⬇️ [Download for macOS, Apple Silicon (MGLTools-1.5.6-Py3-macOS-arm64-Portable.zip, ~92 MB)](https://github.com/Leishem/MGLTools-Py3/releases/download/v1.5.6-py3-macos-arm64/MGLTools-1.5.6-Py3-macOS-arm64-Portable.zip)

![AutoDockTools 4.2 on Windows (Python 3): molecular surface of HIV protease](docs/screenshots/adt_windows_surface.png)

## Download and run (Windows)

1. Download **[`MGLTools-1.5.6-Py3-Win64-Portable.zip`](https://github.com/Leishem/MGLTools-Py3/releases/latest/download/MGLTools-1.5.6-Py3-Win64-Portable.zip)**.
   - Or open the [Releases page](https://github.com/Leishem/MGLTools-Py3/releases/latest),
     scroll to **Assets** and click that zip file.
   - ⚠️ Not the green **`<> Code` → Download ZIP** button, and not the
     **Source code (zip)** / **Source code (tar.gz)** links under Assets —
     those contain only source code and **no `adt.exe`**.
2. Right-click the downloaded zip → **Extract All…** → **Extract**. Any folder
   works, even a USB drive.
3. Open the extracted folder and double-click **`adt.exe`** (AutoDockTools),
   **`pmv.exe`** (molecular viewer) or **`vision.exe`** (Vision).

The folder you get should look like this:

```
MGLTools-1.5.6-Py3-Win64-Portable    adt.exe        <- AutoDockTools
    pmv.exe        <- molecular viewer
    vision.exe     <- Vision
    adt-util.bat   <- command-line preparation scripts
    python\        <- bundled Python (do not delete)
```

If you don't see `adt.exe`, you downloaded the source code instead — go back to step 1.

If Windows SmartScreen shows "Windows protected your PC", click
**More info → Run anyway** (the launchers are not code-signed).

### Supported systems

| Processor | Status |
|---|---|
| Intel / AMD 64-bit (x64) | Native — tested |
| ARM64 (e.g. Qualcomm Snapdragon) | Runs through Windows 11's built-in x64 emulation — expected to work, not yet tested |

Windows 10 or 11 (64-bit) with a current graphics driver (Intel, AMD or NVIDIA).
32-bit Windows is not supported.

### Command-line preparation scripts

From a command prompt in the unzipped folder:

```
adt-util.bat prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
adt-util.bat prepare_ligand4.py   -l ligand.pdb   -o ligand.pdbqt
adt-util.bat prepare_gpf4.py      -l ligand.pdbqt -r receptor.pdbqt -o grid.gpf
adt-util.bat prepare_dpf42.py     -l ligand.pdbqt -r receptor.pdbqt -o dock.dpf
```

Settings are stored in `%USERPROFILE%\.mgltools\1.5.6-py3`, separate from any
original MGLTools 1.5.6 installation.

## Download and run (Linux)

The Linux package (x86_64) runs on the system's **Python 3.12** — the default on
**Ubuntu 24.04**, where it is tested (also under WSL2 on Windows 11). Other
distributions work if they provide Python 3.12 at `/usr/bin/python3.12`.

1. Install the system packages (Ubuntu 24.04 / Debian-based):

   ```
   sudo apt update
   sudo apt install python3.12 python3-tk libgl1 libglu1-mesa
   ```

2. Download and extract
   **[`MGLTools-1.5.6-Py3-Linux-x86_64-Portable.tar.gz`](https://github.com/Leishem/MGLTools-Py3/releases/latest/download/MGLTools-1.5.6-Py3-Linux-x86_64-Portable.tar.gz)**:

   ```
   wget https://github.com/Leishem/MGLTools-Py3/releases/latest/download/MGLTools-1.5.6-Py3-Linux-x86_64-Portable.tar.gz
   tar -xzf MGLTools-1.5.6-Py3-Linux-x86_64-Portable.tar.gz
   cd MGLTools-1.5.6-Py3-Linux-x86_64-Portable
   ```

3. Start a program:

   ```
   ./adt       # AutoDockTools
   ./pmv       # molecular viewer
   ./vision    # Vision
   ```

The folder you get should look like this:

```
MGLTools-1.5.6-Py3-Linux-x86_64-Portable/
    adt  pmv  vision   <- launchers
    adt-util           <- command-line preparation scripts
    python-env/        <- bundled Python packages (do not delete)
```

If a launcher says `Permission denied`, restore the execute bits:
`chmod +x adt pmv vision adt-util python-env/bin/python*`.
The built-in Python shell window needs IDLE (`sudo apt install idle-python3.12`);
without it ADT still runs, with the shell disabled.

Command-line preparation scripts, from the extracted folder:

```
./adt-util prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
./adt-util prepare_ligand4.py   -l ligand.pdb   -o ligand.pdbqt
./adt-util prepare_gpf4.py      -l ligand.pdbqt -r receptor.pdbqt -o grid.gpf
./adt-util prepare_dpf42.py     -l ligand.pdbqt -r receptor.pdbqt -o dock.dpf
```

Run `./adt-util` with no arguments to list all available scripts.

## Download and run (macOS)

The macOS package is for **Apple Silicon** Macs (M1, M2, M3, M4 and later),
macOS 11 Big Sur or newer. It is self-contained: its own Python 3.12, Tk and
libraries are included, so nothing needs to be installed (no Homebrew, no Xcode).

1. Download
   **[`MGLTools-1.5.6-Py3-macOS-arm64-Portable.zip`](https://github.com/Leishem/MGLTools-Py3/releases/download/v1.5.6-py3-macos-arm64/MGLTools-1.5.6-Py3-macOS-arm64-Portable.zip)**
   (or open the [macOS release](https://github.com/Leishem/MGLTools-Py3/releases/tag/v1.5.6-py3-macos-arm64)
   and click the zip under **Assets**) and double-click it to unzip.
2. Move the unzipped folder to your **Applications** folder.
   - ⚠️ Do not run it from **Downloads**, **Desktop** or **Documents**: macOS
     silently blocks apps that are not code-signed from reading files there,
     so nothing would open. If you start it from one of those folders, the app
     explains this and offers to move the folder for you.
3. The first time only: right-click **`Fix-Gatekeeper.command`** → **Open** →
   **Open**. This removes macOS's "downloaded from the internet" block (the
   apps are not code-signed).
4. Double-click **`AutoDockTools.app`**, **`PMV.app`** (molecular viewer) or
   **`Vision.app`** (Vision).

The folder you get should look like this:

```
MGLTools-1.5.6-Py3-macOS-arm64-Portable/
    AutoDockTools.app        <- AutoDockTools
    PMV.app                  <- molecular viewer
    Vision.app               <- Vision
    adt-util.command         <- command-line preparation scripts
    Fix-Gatekeeper.command   <- run once after downloading
    python/                  <- bundled Python (do not delete; keep next to the apps)
```

Errors are written to `~/.mgltools/adt.log` (`pmv.log`, `vision.log`).

Command-line preparation scripts, from Terminal in the folder:

```
./adt-util.command prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
./adt-util.command prepare_ligand4.py   -l ligand.pdb   -o ligand.pdbqt
./adt-util.command prepare_gpf4.py      -l ligand.pdbqt -r receptor.pdbqt -o grid.gpf
./adt-util.command prepare_dpf42.py     -l ligand.pdbqt -r receptor.pdbqt -o dock.dpf
```

### Supported systems and build

| Mac | Status |
|---|---|
| Apple Silicon (M1 and later), macOS 11+ | Native arm64 — tested |
| Intel Macs | Not supported — Apple has moved its whole Mac line to Apple Silicon, so no Intel build is made |

The package was built and tested on:

| | |
|---|---|
| Machine | Apple M1 |
| macOS | 26.5.1 |
| Compiler | Apple clang 15.0.0 (Xcode Command Line Tools) |
| Python | 3.12.15 ([python-build-standalone](https://github.com/astral-sh/python-build-standalone) 20261013) |
| Tk | 9.0.4 |
| Libraries | NumPy 2.5.4, SciPy 1.18.1, PyOpenGL 3.1.10, Pillow 13.0.0, tkinter-gl 1.1 |
| Minimum macOS | 11.0 (deployment target of the compiled modules) |

Rebuild it with `tools/mac/make_portable_macos.sh` (needs the Xcode Command
Line Tools and an internet connection to fetch the bundled Python).

## Reporting a problem

Please [open an issue](../../issues/new/choose) and include:

- **a screenshot** of the problem (and of the Python shell window if an error
  appears there),
- your Windows version and processor (*Settings → System → About*) and graphics card,
  or your Mac model and macOS version (Apple menu → *About This Mac*),
- the steps that led to the problem and, if possible, the input files.

## What was validated

| Check | Result |
|---|---|
| `prepare_receptor4`, `prepare_ligand4`, `prepare_gpf4`, `prepare_dpf42` on the ADT tutorial (HIV protease hsg1 + indinavir) | **byte-identical** output to the original MGLTools 1.5.6, on Windows, Linux and macOS |
| Molecular surface vs the original MSMS | vertices within ~0.1 Å of MSMS's surface; area within ~3 % |
| GUI | loads molecules; lines, sticks & balls, CPK, ribbons, molecular surfaces, selection and colouring render correctly |

| CPK | Ribbon |
|---|---|
| ![CPK](docs/screenshots/adt_windows_cpk.png) | ![Ribbon](docs/screenshots/adt_windows_ribbon.png) |

## How it was ported

* **Python 2 → 3**: automated conversion plus compatibility shims for Python 2
  behaviour the code relies on (`src/mglcompat/`).
* **Native code**: the 15 C/C++ extension modules were rebuilt for Python 3 and
  NumPy 2 with SWIG 4 (`native/`).
* **OpenGL**: the old Togl widget and SWIG GL bindings were replaced by
  [TkGL](https://github.com/3-manifolds/tkinter_gl) and PyOpenGL.
* **Molecular surfaces**: MSMS was available only as closed binaries for old
  platforms; `src/mslib/` is a new open-source, MSMS-compatible implementation.

### Building from source

```
pip install .                       # Windows needs Visual Studio Build Tools
adt                                 # or pmv / vision
python -m pytest                    # test suite (compares with MGLTools 1.5.6 outputs)
```

Portable packages: Linux x86_64 is published with each release (see
[Download and run (Linux)](#download-and-run-linux)); macOS Apple Silicon is
built with `tools/mac/make_portable_macos.sh` (see
[Download and run (macOS)](#download-and-run-macos)).

## Known limitations

* Surfaces: channels narrower than the surface grid (~0.6 Å at the default
  density) may be closed, so a few internal-pocket patches can be missing.
* macOS: the apps are not code-signed or notarized, hence the one-time
  Gatekeeper step and the Downloads/Desktop/Documents restriction.
* Not included: the GROMACS `.xtc` trajectory reader on Windows, web-service
  (Opal/ZSI) clients, and the Blender/Cinema 4D/Maya plugins.

## License

MGLTools is © The Scripps Research Institute and is **free for non-commercial
educational and academic research use**; some components (e.g. the UT
packages) carry their own academic terms. See [`LICENSES`](LICENSES). This
port does not change those terms. For commercial use, contact the original
authors.

Original software: [MGLTools](https://ccsb.scripps.edu/mgltools/) by the
Molecular Graphics Laboratory, The Scripps Research Institute.
