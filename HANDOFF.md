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

## Open issue being debugged (Windows rendering)
On Windows (Intel RaptorLake GPU, GL 4.6) **filled geometry in PMV (CPK spheres, ribbons,
molecular surface) renders as a regular screen-space dot pattern**; lines render fine.
Plain DejaVu (no PMV) renders solid spheres correctly, incl. a second camera sharing
display lists (so list sharing works). Ruled out: SSAO, selection contour, selection
pattern (DejaVu.selectionPatternSize=0), highlight arrays (empty), polygon mode/front face/
culling (identical to Linux), pixel format (RGBA8, depth24, stencil8), screen-capture artifact
(GL framebuffer read shows it too). Last check: the Intel workaround flag override does take
effect (intelBlack/White False) — but run with MGL_NO_INTEL_WORKAROUND still showed dots, so
NOT the Intel workaround.
Next ideas: (1) GL_POLYGON_STIPPLE / GL_ALPHA_TEST state *during* draw — record GL calls
(apitrace or wrap glEnable); (2) PMV-specific setup absent in plain DejaVu: material editor /
colormap GL widgets created in other contexts, `transparent`/opacity handling, `_pmvrc`
settings; (3) bisect by building the PMV scene in plain DejaVu step by step.
Debug switches in tests/gui_smoke.py: MGL_NO_SSAO, MGL_NO_CONTOUR, MGL_NO_PATTERN,
MGL_NO_INTEL_WORKAROUND. Reproduce: `build/gui-win*/3_cpk.png` from
`D:\Reverse_Engineer\venv-wintest\Scripts\python.exe tests\gui_smoke.py build\gui-winN`
(rebuild wheel first with tools/build_win.ps1, or copy changed .py into venv-wintest site-packages).

## Left to do
1. Fix the Windows dot-rendering issue above.
2. GitHub: user must run `gh auth login` (GitHub CLI at C:\Program Files\GitHub CLI\gh.exe),
   then create a **private** repo, push `main`, watch CI (`gh run watch`); fix macOS build errors
   (macOS GL forwarding headers in native/common/macos are untested).
3. Windows ARM64: needs arm.yml (public repo / paid ARM runners) or test x64 build under emulation.
4. Rebuild WSL in-place natives after .i changes (`tools/build_native.sh`), rerun
   `MGL_USE_SRC=1 python -m pytest` and tests/gui_smoke.sh.
5. Optional: remaining smoke-import failures are optional deps (ZSI web services, matplotlib, OpenCSG).

## Environments
- WSL Ubuntu 24.04 venv `~/mglenv` (py3.12); original 1.5.6 reference at `~/mgl156`.
- Windows: `D:\Reverse_Engineer\venv-win` (build deps incl. swig), `venv-wintest` (installed wheel).
- MSVC Build Tools 2022 + Windows SDK installed.
- Helper scripts: tools/wsl.sh, tools/xrun.sh, tools/build_native.sh, tools/build_wheel.sh,
  tools/build_win.ps1, tests/compare_adt.sh, tests/compare_msms.sh (vs ~/mgl156).
