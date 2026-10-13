"""Start the ADT/PMV GUI, load a protein, render it and save screenshots.

Usage: python tests/gui_smoke.py [out_dir] [pdb]
Run under a display (tools/xrun.sh uses Xvfb with FORCE_XVFB=1).
Exit status 0 if every step succeeded.
"""
import faulthandler
import os
import pathlib
import sys
import time
import traceback

faulthandler.enable()

ROOT = pathlib.Path(__file__).resolve().parent.parent
if os.environ.get("MGL_USE_SRC"):
    sys.path.insert(0, str(ROOT / "src"))
import Pmv  # noqa: E402
OUT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ROOT / "build" / "gui")
PDB = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(Pmv.__file__), "doc", "hsg1.pdb")
OUT.mkdir(parents=True, exist_ok=True)

failures = []
t0 = time.time()


def step(name, fn):
    t = time.time()
    try:
        r = fn()
        print(f"OK    {name:38} {time.time() - t:6.2f}s", flush=True)
        return r
    except Exception:
        failures.append(name)
        print(f"FAIL  {name:38} {time.time() - t:6.2f}s", flush=True)
        traceback.print_exc()
        return None


def make_viewer():
    from tkinter import Tk
    root = Tk(sync=bool(os.environ.get("MGL_X11_SYNC")))
    root.withdraw()
    from mglcompat import x11errors
    x11errors.install()
    from Pmv.moleculeViewer import MoleculeViewer
    mv = MoleculeViewer(logMode='no', customizer=None, master=root,
                        title="AutoDockTools", withShell=0, verbose=False, gui=True)
    return mv


mv = step("create MoleculeViewer (PMV GUI)", make_viewer)
if mv is None:
    sys.exit(1)

for mod in ("autotors41Commands", "autogpf41Commands", "autodpf41Commands",
            "autostart41Commands", "autoanalyze41Commands"):
    step(f"load ADT {mod}",
         lambda m=mod: mv.browseCommands(m, commands=None, package='AutoDockTools'))

def show_window():
    # what runADT does once everything is built
    from AutoDockTools import setADTmode
    setADTmode("AD4.2", mv)
    mv.GUI.naturalSize()
    mv.GUI.ROOT.deiconify()
    mv.GUI.ROOT.update()


step("show main window (setADTmode, deiconify)", show_window)

info = step("GL context info", lambda: {
    "vendor": __import__("opengltk.OpenGL.GL", fromlist=["x"]).glGetString(0x1F00),
    "renderer": __import__("opengltk.OpenGL.GL", fromlist=["x"]).glGetString(0x1F01),
    "version": __import__("opengltk.OpenGL.GL", fromlist=["x"]).glGetString(0x1F02),
})
if info:
    print("      ", info)

mols = step("read " + os.path.basename(PDB), lambda: mv.readMolecule(PDB))
cam = mv.GUI.VIEWER.currentCamera


def shot(name):
    mv.GUI.VIEWER.Redraw()
    mv.GUI.ROOT.update()
    img = cam.GrabFrontBuffer()
    p = OUT / f"{name}.png"
    img.save(p)
    return p


if mols:
    step("display lines + screenshot", lambda: shot("1_lines"))
    step("displaySticksAndBalls", lambda: mv.displaySticksAndBalls("hsg1", log=0))
    step("sticks screenshot", lambda: shot("2_sticks"))
    step("displayCPK", lambda: mv.displayCPK("hsg1", log=0))
    step("CPK screenshot", lambda: shot("3_cpk"))
    step("ribbon (secondary structure)", lambda: mv.ribbon("hsg1", log=0))
    step("ribbon screenshot", lambda: shot("4_ribbon"))
    step("select residues ARG8", lambda: mv.select("hsg1::ARG8", log=0))
    step("color by atom type", lambda: mv.colorByAtomType("hsg1", ['lines'], log=0))
    step("undisplay CPK", lambda: mv.displayCPK("hsg1", negate=True, log=0))
    step("computeMSMS (molecular surface)",
         lambda: mv.computeMSMS("hsg1", surfName="MSMS-MOL", pRadius=1.5,
                                density=3.0, log=0))
    step("surface area info", lambda: print(
        "      ", mv.GUI.VIEWER and
        mv.Mols[0].geomContainer.msms["MSMS-MOL"][0].info()))
    step("color surface by atom type",
         lambda: mv.colorByAtomType("hsg1", ['MSMS-MOL'], log=0))

def window_shot():
    mv.GUI.ROOT.update()
    from PIL import ImageGrab
    r = mv.GUI.ROOT
    box = (r.winfo_rootx(), r.winfo_rooty(),
           r.winfo_rootx() + r.winfo_width(), r.winfo_rooty() + r.winfo_height())
    try:
        img = ImageGrab.grab(bbox=box)
    except Exception:  # noqa: BLE001  (e.g. Pillow without XCB)
        img = ImageGrab.grab()
    img.save(OUT / "0_window.png")


step("full window screenshot", window_shot)
print(f"\n{len(failures)} failures in {time.time() - t0:.1f}s; screenshots in {OUT}")
try:
    mv.Exit(0)
except SystemExit:
    pass
except Exception:
    pass
os._exit(1 if failures else 0)
