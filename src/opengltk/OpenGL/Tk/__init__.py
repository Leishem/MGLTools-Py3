"""Tk OpenGL widget support.

MGLTools 1.5.6 bundled Togl 1.7/2.0 built against Tk 8.4 private headers.
This port uses TkGL (a maintained Togl 2 descendant from the tkinter-gl
package) which has the same widget options and subcommands (makecurrent,
swapbuffers, contexttag, ...) and supports Tk 8.6 and 9 on macOS (Cocoa),
Windows (WGL) and Linux (GLX).
"""
import os
import platform
import sys

WIDGET_CLASS = "tkgl"


def tkgl_dir():
    """Directory holding the TkGL Tcl package for this platform."""
    override = os.environ.get("MGL_TKGL_DIR")
    if override:
        return override
    import tkinter_gl
    target = sys.platform
    if target.startswith("linux"):
        target = "linux-" + platform.machine()
    return os.path.join(os.path.dirname(tkinter_gl.__file__), "tk", target)


def loadTkgl(master):
    """Load TkGL into master's Tcl interpreter; return the package version."""
    d = tkgl_dir()
    if not os.path.isdir(d):
        raise RuntimeError("TkGL package directory %r is missing; install "
                           "tkinter-gl or set MGL_TKGL_DIR" % d)
    auto_path = master.tk.splitlist(master.tk.globalgetvar('auto_path'))
    if d not in auto_path:
        master.tk.call('lappend', 'auto_path', d)
    return master.tk.call('package', 'require', 'Tkgl')
