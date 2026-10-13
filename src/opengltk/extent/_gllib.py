"""Raw GL entry points (C argument order), formerly the SWIG module _gllib."""
import opengltk  # noqa: F401  (sets PyOpenGL platform/flags first)
from OpenGL.GL import *  # noqa: F401,F403
from opengltk import _pyogl

for _n in _pyogl.RAW_OVERRIDES:
    globals()[_n] = getattr(_pyogl, _n)
del _n
_pyogl.plain_constants(globals())
