"""Raw GLU entry points, formerly the SWIG module _glulib."""
import opengltk  # noqa: F401
from OpenGL.GLU import *  # noqa: F401,F403
from opengltk import _pyogl
_pyogl.plain_constants(globals())
