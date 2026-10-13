"""Create PMV's IDLE-based Python shell on its own (traceback stays on stderr)."""
import os
import sys
import tkinter
import traceback

if os.environ.get("MGL_USE_SRC"):
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "src"))
err = sys.stderr
root = tkinter.Tk()
try:
    from mglutil.util.idleUtil import getShell
    sh = getShell(None, rootTk=root, enable_shell=True, enable_edit=False)
    root.update()
    sys.stdout = sys.__stdout__
    sys.stderr = err
    print("SHELL_OK", type(sh).__name__)
except Exception:
    sys.stderr = err
    traceback.print_exc()
    sys.exit(1)
