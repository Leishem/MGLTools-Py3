#
# copyright_notice
#
'''opengltk exception processing'''

__all__ = ()

from OpenGL.error import GLError as GLerror  # noqa: F401  (PyOpenGL raises this)

from .extent import _gllib
from .OpenGL.GLU import gluErrorString

opengltk_exception_maxerrs = 10


def processglerror(errcode):
    '''callback from C function util_processerror() in _utillib'''
    errs = [errcode]
    othererr = _gllib.glGetError()
    while othererr and othererr != _gllib.GL_INVALID_OPERATION:
        errs.append(othererr)
        if opengltk_exception_maxerrs < len(errs):
            raise SystemError('opengltk_exception_maxerrs reached',
                              list(map(gluErrorString, errs)))
        othererr = _gllib.glGetError()
    raise RuntimeError('GL error: ' + ', '.join(map(str, map(gluErrorString, errs))))


class GLwarning(RuntimeWarning):
    pass


def gluErrorCallback(errorCode):
    '''default callback for glu error functions'''
    import warnings
    warnings.warn(gluErrorString(errorCode), GLwarning)
