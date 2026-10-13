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

# Per-call glGetError checking is opt-in (MGL_GL_ERROR_CHECKING=1). DejaVu
# issues GL calls on widgets before Tk maps them; old drivers ignored those,
# Mesa/modern drivers flag GL_INVALID_FRAMEBUFFER_OPERATION, and PyOpenGL
# would turn each into an exception. Off is also much faster.
OpenGL.ERROR_CHECKING = os.environ.get("MGL_GL_ERROR_CHECKING", "0") == "1"
OpenGL.ERROR_ON_COPY = False

CRITICAL_DEPENDENCIES = ['numpy', 'OpenGL']
NONCRITICAL_DEPENDENCIES = ['mglutil']
