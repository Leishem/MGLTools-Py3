# MGLTools 1.5.6 for Python 3 — AutoDockTools GUI, portable for Windows

**AutoDockTools (ADT)**, the **Python Molecular Viewer (PMV)** and **Vision** from
MGLTools 1.5.6, ported from Python 2.5 / Numeric / Togl to **Python 3 /
NumPy 2 / modern OpenGL**.

The Windows release is **portable**: no installation, no Python needed.
Download, unzip, double-click `adt.exe`.

![AutoDockTools 4.2 on Windows (Python 3): molecular surface of HIV protease](docs/screenshots/adt_windows_surface.png)

## Download and run (Windows)

1. Download **`MGLTools-1.5.6-Py3-Win64-Portable.zip`** from the
   [Releases page](../../releases/latest).
2. Unzip it anywhere — a local folder or a USB drive.
3. Double-click **`adt.exe`** (AutoDockTools), **`pmv.exe`** (molecular viewer)
   or **`vision.exe`** (Vision).

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

## Reporting a problem

Please [open an issue](../../issues/new/choose) and include:

- **a screenshot** of the problem (and of the Python shell window if an error
  appears there),
- your Windows version and processor (*Settings → System → About*) and graphics card,
- the steps that led to the problem and, if possible, the input files.

## What was validated

| Check | Result |
|---|---|
| `prepare_receptor4`, `prepare_ligand4`, `prepare_gpf4`, `prepare_dpf42` on the ADT tutorial (HIV protease hsg1 + indinavir) | **byte-identical** output to the original MGLTools 1.5.6, on Windows and Linux |
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

Linux and macOS builds are supported by the source; release packages for them
are not published yet.

## Known limitations

* Surfaces: channels narrower than the surface grid (~0.6 Å at the default
  density) may be closed, so a few internal-pocket patches can be missing.
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
