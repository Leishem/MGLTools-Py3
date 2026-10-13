"""Create a TkGL widget with a given set of options; print the GL version.

Usage: python tests/tkgl_probe.py [-opt value ...]
Each run is a separate process because a GLX error aborts the process.
"""
import pathlib
import sys
import tkinter

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "src"))
from opengltk.OpenGL.Tk import loadTkgl  # noqa: E402

opts = dict(zip((a.lstrip("-") for a in sys.argv[1::2]), sys.argv[2::2]))
root = tkinter.Tk()
print("Tkgl", loadTkgl(root), "opts", opts, flush=True)
if sys.platform == "win32":
    root.update()  # TkGL's WGL backend needs the parent window to exist
w = tkinter.Widget(root, "tkgl", {}, dict(width=200, height=200, **opts))
w.pack()
root.update()
w.tk.call(w._w, "makecurrent")
from opengltk.OpenGL import GL  # noqa: E402
print("GL_VERSION", GL.glGetString(GL.GL_VERSION), "|", GL.glGetString(GL.GL_RENDERER))
root.destroy()
