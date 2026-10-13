"""Non-fatal handling of GL-related X errors on X11 (Linux).

Xlib's default error handler exits the process. A stray GLX error (e.g. a
buffer swap on a window that is being destroyed) would therefore kill PMV,
where the old Togl/driver stack tolerated it. install() chains a handler in
front of Tk's: errors raised by X *extension* requests (GLX and friends,
request codes >= 128) are ignored, all others go to the previous handler.

MGL_X11_VERBOSE=1 logs each ignored error with the Python stack; combine with
tkinter.Tk(sync=True) so the stack points at the failing call.
"""
import ctypes
import ctypes.util
import os
import sys
import traceback

_installed = None


class _XErrorEvent(ctypes.Structure):
    _fields_ = [("type", ctypes.c_int),
                ("display", ctypes.c_void_p),
                ("resourceid", ctypes.c_ulong),
                ("serial", ctypes.c_ulong),
                ("error_code", ctypes.c_ubyte),
                ("request_code", ctypes.c_ubyte),
                ("minor_code", ctypes.c_ubyte)]


_HANDLER = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.POINTER(_XErrorEvent))


def install():
    """Install the handler (no-op off Linux or without libX11). Call after
    Tk has opened its display, so Tk's handler is the one being chained."""
    global _installed
    if _installed is not None or not sys.platform.startswith("linux"):
        return
    path = ctypes.util.find_library("X11")
    if not path:
        return
    x11 = ctypes.CDLL(path)
    x11.XSetErrorHandler.restype = ctypes.c_void_p
    x11.XSetErrorHandler.argtypes = [_HANDLER]
    verbose = os.environ.get("MGL_X11_VERBOSE")
    state = {}

    def handler(display, event):
        e = event.contents
        if e.request_code >= 128:
            if verbose:
                sys.stderr.write(
                    "[x11errors] ignored X error code=%d request=%d.%d resource=0x%x\n"
                    % (e.error_code, e.request_code, e.minor_code, e.resourceid))
                traceback.print_stack(limit=12)
            return 0
        prev = state.get("previous")
        if prev:
            return _HANDLER(prev)(display, event)
        return 0

    cb = _HANDLER(handler)
    state["previous"] = x11.XSetErrorHandler(cb)
    _installed = (x11, cb, state)  # keep references alive
