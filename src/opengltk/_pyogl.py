"""Adapters between opengltk's conventions and PyOpenGL.

opengltk (SWIG) conventions that MGLTools code relies on:
  * glGet{Integer,Float,Double,Boolean}v return a flat array, even for a
    single value (code does ``glGetIntegerv(GL_DRAW_BUFFER)[0]``)
  * glGenTextures(n) returns an array; glGetString returns str
  * raw calls pass an output NumPy array (glReadPixels(..., nar)) or an
    integer VBO offset as the pointer argument (glVertexPointer(3, T, 0, 0))
  * glSelectBuffer(size) returns the hit buffer as an array and
    glRenderMode() returns the plain hit count
"""
import ctypes

import numpy as np
from OpenGL import GL as _GL
from OpenGL.raw.GL.VERSION import GL_1_0 as _raw10
from OpenGL.raw.GL.VERSION import GL_1_1 as _raw11

# Arrays handed to GL by pointer must outlive the call (vertex arrays are
# read at draw time), so keep the last one per entry point alive.
_keep = {}


def ptr(name, x, dtype=None):
    """ctypes pointer for x: None/int -> offset, array-like -> data pointer."""
    if x is None:
        return None
    if isinstance(x, (int, np.integer)):
        return ctypes.c_void_p(int(x))
    a = np.asarray(x, dtype=dtype)
    if not a.flags.c_contiguous:
        a = np.ascontiguousarray(a)
    _keep[name] = a
    return ctypes.c_void_p(a.ctypes.data)


# numpy dtype char -> GL type enum
GLTYPE = {
    'b': _GL.GL_BYTE, 'B': _GL.GL_UNSIGNED_BYTE,
    'h': _GL.GL_SHORT, 'H': _GL.GL_UNSIGNED_SHORT,
    'i': _GL.GL_INT, 'I': _GL.GL_UNSIGNED_INT,
    'l': _GL.GL_INT, 'L': _GL.GL_UNSIGNED_INT,
    'f': _GL.GL_FLOAT, 'd': _GL.GL_DOUBLE,
}
NPTYPE = {
    _GL.GL_BYTE: np.int8, _GL.GL_UNSIGNED_BYTE: np.uint8,
    _GL.GL_SHORT: np.int16, _GL.GL_UNSIGNED_SHORT: np.uint16,
    _GL.GL_INT: np.int32, _GL.GL_UNSIGNED_INT: np.uint32,
    _GL.GL_FLOAT: np.float32, _GL.GL_DOUBLE: np.float64,
}


def gltype_of(a):
    a = np.asarray(a)
    if a.dtype.char in 'lL' and a.dtype.itemsize == 8:
        raise TypeError("64-bit integer arrays cannot be passed to OpenGL; "
                        "use int32")
    return GLTYPE[a.dtype.char]


def flat(value, dtype):
    return np.ravel(np.asarray(value, dtype=dtype))


# --- glGet*v: always flat arrays ---------------------------------------------
def glGetIntegerv(pname):
    return flat(_GL.glGetIntegerv(pname), np.int32)


def glGetFloatv(pname):
    return flat(_GL.glGetFloatv(pname), np.float32)


def glGetDoublev(pname):
    return flat(_GL.glGetDoublev(pname), np.float64)


def glGetBooleanv(pname):
    return flat(_GL.glGetBooleanv(pname), np.uint8)


def glGetString(name):
    s = _GL.glGetString(name)
    return s.decode("latin-1") if isinstance(s, bytes) else (s or "")


def glGenTextures(n):
    out = np.zeros(n, np.uint32)
    _raw11.glGenTextures(n, out.ctypes.data_as(ctypes.POINTER(ctypes.c_uint)))
    return out


def glDeleteTextures(*args):
    seq = args[-1]
    a = np.ascontiguousarray(np.atleast_1d(seq), dtype=np.uint32)
    _raw11.glDeleteTextures(len(a), a.ctypes.data_as(ctypes.POINTER(ctypes.c_uint)))


# --- selection ----------------------------------------------------------------
def glSelectBuffer(size, buffer=None):
    if buffer is None:
        buffer = np.zeros(size, np.uint32)
    _keep["glSelectBuffer"] = buffer
    _raw10.glSelectBuffer(size, buffer.ctypes.data_as(ctypes.POINTER(ctypes.c_uint)))
    return buffer


def glFeedbackBuffer(size, type, buffer=None):
    if buffer is None:
        buffer = np.zeros(size, np.float32)
    _keep["glFeedbackBuffer"] = buffer
    _raw10.glFeedbackBuffer(size, type, buffer.ctypes.data_as(ctypes.POINTER(ctypes.c_float)))
    return buffer


def glRenderMode(mode):
    return int(_raw10.glRenderMode(mode))


# --- raw C-style signatures with output arrays / offsets ----------------------
def glReadPixels(x, y, width, height, format, type, pixels=None):
    if pixels is None:
        return _GL.glReadPixels(x, y, width, height, format, type)
    a = pixels if isinstance(pixels, np.ndarray) else np.asarray(pixels)
    if not a.flags.c_contiguous:
        raise ValueError("glReadPixels output array must be contiguous")
    _raw10.glReadPixels(x, y, width, height, format, type, ctypes.c_void_p(a.ctypes.data))
    return a


def glDrawPixels(width, height, format, type, pixels):
    _raw10.glDrawPixels(width, height, format, type, ptr("glDrawPixels", pixels))


def glTexImage2D(target, level, internalformat, width, height, border, format,
                 type, pixels):
    _raw10.glTexImage2D(target, level, internalformat, width, height, border,
                        format, type, ptr("glTexImage2D", pixels))


def glTexImage1D(target, level, internalformat, width, border, format, type, pixels):
    _raw10.glTexImage1D(target, level, internalformat, width, border, format,
                        type, ptr("glTexImage1D", pixels))


def glVertexPointer(size, type, stride, pointer):
    _raw11.glVertexPointer(size, type, stride, ptr("glVertexPointer", pointer))


def glNormalPointer(type, stride, pointer):
    _raw11.glNormalPointer(type, stride, ptr("glNormalPointer", pointer))


def glColorPointer(size, type, stride, pointer):
    _raw11.glColorPointer(size, type, stride, ptr("glColorPointer", pointer))


def glTexCoordPointer(size, type, stride, pointer):
    _raw11.glTexCoordPointer(size, type, stride, ptr("glTexCoordPointer", pointer))


def glDrawElements(mode, count, type, indices):
    _raw11.glDrawElements(mode, count, type, ptr("glDrawElements", indices))


def glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap):
    _raw10.glBitmap(width, height, xorig, yorig, xmove, ymove,
                    ptr("glBitmap", bitmap, np.uint8))


def plain_constants(ns):
    """Replace PyOpenGL Constant objects in namespace ns with plain numbers.

    PyOpenGL constants are int subclasses whose str() is the symbol name, so
    storing one in a Tk IntVar stores "GL_SRC_ALPHA (GL_SRC_ALPHA)".
    opengltk's constants were plain ints."""
    from OpenGL.constant import Constant
    for k, v in list(ns.items()):
        if isinstance(v, Constant):
            ns[k] = float(v) if isinstance(v, float) else int(v)


RAW_OVERRIDES = [
    "glGetIntegerv", "glGetFloatv", "glGetDoublev", "glGetBooleanv",
    "glGetString", "glGenTextures", "glDeleteTextures", "glSelectBuffer",
    "glFeedbackBuffer", "glRenderMode", "glReadPixels", "glDrawPixels",
    "glTexImage2D", "glTexImage1D", "glVertexPointer", "glNormalPointer",
    "glColorPointer", "glTexCoordPointer", "glDrawElements", "glBitmap",
]
