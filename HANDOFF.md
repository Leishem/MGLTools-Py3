# Handoff — MGLTools 1.5.6 → Python 3 port (status 2026-10-13)

Repo: `D:\Reverse_Engineer\mgltools-py3` (git, branch `main`). Read README.md first.
Goal: MGLTools 1.5.6 (ADT + PMV GUI) running natively on macOS arm64, Linux,
Windows x64/ARM64. User chose: full GUI scope, **private** GitHub repo, CI via GitHub Actions.

## Done and verified
- All ~830 Python files converted to Python 3 (tools/convert_py.py, fix_py3_misc.py,
  fix_code_strings.py, convert_division.sh, convert_extensionless.sh); compat shims in src/mglcompat/.
- All 15 native SWIG modules build on **Linux (gcc, WSL)** and **Windows x64 (MSVC)**.
- ADT pipeline (prepare_receptor4/ligand4/gpf4/dpf42) **byte-identical** to original 1.5.6
  on Linux and Windows (tests/test_adt_pipeline.py, golden files tests/data/adt_ref_156).
- MSMS (closed binary) replaced by open-source grid SES in src/mslib/ (validated vs MSMS: sd 0.12 Å, area −3%).
- OpenGL: PyOpenGL + adapters (src/opengltk/_pyogl.py, OpenGL/GL.py); Tk widget = TkGL from tkinter-gl.
- Linux GUI: PMV/ADT renders correctly (lines, sticks, CPK, ribbon, surface); real `adt` launcher starts
  cleanly incl. IDLE shell + Vision (tests/launch_check.sh, tests/gui_smoke.sh under Xvfb).
- Windows x64: wheel builds, **24/24 pytest pass** on installed wheel (tools/build_win.ps1),
  GUI starts, all commands succeed (tests/gui_smoke.py, 0 failures).
- Packaging: pyproject.toml + setup.py, wheel with `adt`/`pmv`/`vision` entry points.
- CI written: .github/workflows/ci.yml (Linux x86_64, macOS arm64, Windows x64),
  arm.yml (manual; ARM runners need public repo), build-one.yml (reusable).

## Resolved Issues
### 1. Windows Dot-Rendering / Screendoor Pattern (FIXED)
- **Root cause**: `MeasureAngleGUICommand` (`Fan3D('angles')`) and `MeasureTorsionGUICommand` (`IndexedPolygons('torsionLine')`) in `src/Pmv/measureCommands.py` were initialized with `stipplePolygons=1` and `polygonstipple.Set(pattern=pat3)`. In `DejaVu/bitPatterns.py`, `pat3` is `(0xAA, 0x00, 0xAA, 0x00)` — the exact 25% screen-door dot pattern. Because `glPushAttrib(GL_POLYGON_BIT)` does not preserve `GL_POLYGON_STIPPLE` enable state (which resides in `GL_ENABLE_BIT`), `GL_POLYGON_STIPPLE` remained permanently enabled across display list compilation and subsequent draw calls, contaminating all filled geometry (CPK spheres, ribbons, SES surface).
- **Fixes**:
  1. `src/Pmv/measureCommands.py`: Default `stipplePolygons=0` on `self.fan` and `self.lines`.
  2. `src/DejaVu/Camera.py`: Added explicit `GL.glDisable(GL.GL_POLYGON_STIPPLE)` at the end of `Camera.Draw()` whenever an object had polygon stippling enabled, ensuring no state leakage across the scene graph.

### 2. Secondary Structure & Tree Sorting Regressions (FIXED)
- `src/MolKit/tree.py`: `TreeNodeSet.sort(self, func=None, key=None)` updated to support the `key=` keyword parameter. In Python 3, `cmp_to_key` passes `key=...` which raised `TypeError: TreeNodeSet.sort() got an unexpected keyword argument 'key'`.
- `src/MolKit/getsecondarystructure.py`: Replaced `.has_key` calls with `.__contains__`.
- `src/AutoDockTools/autotorsCommands.py`: Guarded `AD4.2` command delegations (`AD41flex_setResidues`, etc.) with `hasattr(self.vf, ...)`.
- Fixed lingering `.has_key` calls in `src/Vision/UserLibBuild.py`, `src/Vision/WebNodes.py`, `src/Vision/matplotlibNodes.py`, `src/DejaVu/VisionInterface/DejaVuNodes.py`, `src/MolKit/VisionInterface/MolKitNodes.py`, `src/Volume/VisionInterface/VolumeNodes.py`.

## Left to do
1. GitHub: user must run `gh auth login` (GitHub CLI at C:\Program Files\GitHub CLI\gh.exe),
   then create a **private** repo, push `main`, watch CI (`gh run watch`); fix macOS build errors
   (macOS GL forwarding headers in native/common/macos are untested).
2. Windows ARM64: needs arm.yml (public repo / paid ARM runners) or test x64 build under emulation.
3. Rebuild WSL in-place natives after .i changes (`tools/build_native.sh`), rerun
   `MGL_USE_SRC=1 python -m pytest` and tests/gui_smoke.sh.
4. Optional: remaining smoke-import failures are optional deps (ZSI web services, matplotlib, OpenCSG).

## Environments
- WSL Ubuntu 24.04 venv `~/mglenv` (py3.12); original 1.5.6 reference at `~/mgl156`.
- Windows: `D:\Reverse_Engineer\venv-win` (build deps incl. swig), `venv-wintest` (installed wheel).
- MSVC Build Tools 2022 + Windows SDK installed.
- Helper scripts: tools/wsl.sh, tools/xrun.sh, tools/build_native.sh, tools/build_wheel.sh,
  tools/build_win.ps1, tests/compare_adt.sh, tests/compare_msms.sh (vs ~/mgl156).
