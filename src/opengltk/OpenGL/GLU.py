#
# copyright_notice
#
"""GLU module: PyOpenGL GLU with opengltk's gluProject/gluUnProject form,
which takes the point as one 3-sequence and returns a 3-array."""
import numpy as np

from opengltk.extent._glulib import *  # noqa: F401,F403
from OpenGL import GLU as _GLU


def _mpv(model, proj, view):
    return (np.asarray(model, np.float64).reshape(4, 4),
            np.asarray(proj, np.float64).reshape(4, 4),
            np.asarray(view, np.int32).ravel()[:4])


def gluProject(obj, *args):
    if np.ndim(obj) == 0:  # C form (x, y, z, model, proj, view)
        return _GLU.gluProject(obj, *args)
    model, proj, view = _mpv(*args)
    return np.array(_GLU.gluProject(obj[0], obj[1], obj[2], model, proj, view),
                    np.float64)


def gluUnProject(win, *args):
    if np.ndim(win) == 0:
        return _GLU.gluUnProject(win, *args)
    model, proj, view = _mpv(*args)
    return np.array(_GLU.gluUnProject(win[0], win[1], win[2], model, proj, view),
                    np.float64)


def gluPickMatrix(x, y, delX, delY, view):
    _GLU.gluPickMatrix(x, y, delX, delY, np.asarray(view, np.int32).ravel()[:4])


def gluErrorString(code):
    s = _GLU.gluErrorString(code)
    return s.decode("latin-1") if isinstance(s, bytes) else s
