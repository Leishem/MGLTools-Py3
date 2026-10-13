# Sourced by ADT/PMV at startup (``adt -s tests/launch_check.py``): load a
# molecule, take a screenshot after the GUI settles, then exit.
import os
import Pmv
from PIL import ImageGrab

out = os.environ.get("MGL_LAUNCH_OUT", "launch.png")
self.readMolecule(os.path.join(os.path.dirname(Pmv.__file__), "doc", "hsg1.pdb"))  # noqa: F821
self.displaySticksAndBalls("hsg1", log=0)  # noqa: F821


def _shot_and_quit(mv=self, out=out, ImageGrab=ImageGrab, os=os):  # noqa: F821
    try:
        r = mv.GUI.ROOT
        r.update()
        try:
            img = ImageGrab.grab(bbox=(r.winfo_rootx(), r.winfo_rooty(),
                                 r.winfo_rootx() + r.winfo_width(),
                                 r.winfo_rooty() + r.winfo_height()))
        except Exception:
            try:
                img = ImageGrab.grab()
            except Exception:
                img = mv.GUI.VIEWER.currentCamera.GrabFrontBuffer()
        img.save(out)
        print("LAUNCH_OK", out, flush=True)
    except Exception as e:
        print("LAUNCH_ERROR", e, flush=True)
    finally:
        os._exit(0)


self.GUI.ROOT.after(4000, _shot_and_quit)  # noqa: F821
