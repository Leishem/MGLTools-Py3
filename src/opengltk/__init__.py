#
# copyright_notice
#
"""opengltk on PyOpenGL.

The original opengltk shipped SWIG bindings for GL/GLU/GLX plus the Togl
Tk widget. In this port the GL and GLU entry points come from PyOpenGL
(see opengltk._pyogl for the adapters that keep opengltk's calling and
return conventions) and the Tk widget is TkGL from the tkinter-gl package.
Only the DejaVu C helpers (extent._utillib) remain a native extension.
"""
import os
import sys

if sys.platform.startswith("linux"):
    # TkGL creates GLX contexts; PyOpenGL must resolve entry points via GLX
    # (it would pick EGL under Wayland otherwise).
    os.environ.setdefault("PYOPENGL_PLATFORM", "glx")

import OpenGL  # noqa: E402

# opengltk raised on GL errors after every call; keep that by default but
# allow turning it off for speed (MGL_GL_ERROR_CHECKING=0).
OpenGL.ERROR_CHECKING = os.environ.get("MGL_GL_ERROR_CHECKING", "1") != "0"
OpenGL.ERROR_ON_COPY = False

CRITICAL_DEPENDENCIES = ['numpy', 'OpenGL']
NONCRITICAL_DEPENDENCIES = ['mglutil']
