#
# copyright_notice
#
"""GL module: PyOpenGL with opengltk's Python-level signatures.

Functions whose opengltk signature differs from the C one (e.g.
glVertexPointer(size, pointer, stride=0, ctype=None)) accept both forms,
so code written against either opengltk.OpenGL.GL or _gllib keeps working.
"""
import numpy as np

from opengltk.extent._gllib import *  # noqa: F401,F403
from opengltk import _pyogl as _p


def _is_enum(x):
    return isinstance(x, (int, np.integer))


def glCallLists(lists, *rest):
    if rest:  # C form (n, type, lists)
        n, type_, data = lists, rest[0], rest[1]
        return _p._raw10.glCallLists(n, type_, _p.ptr("glCallLists", data))
    a = np.ascontiguousarray(lists)
    _p._raw10.glCallLists(len(a), _p.gltype_of(a), _p.ptr("glCallLists", a))


def glColorPointer(size, pointer, stride=0, gltype=None):
    if _is_enum(pointer) and not isinstance(stride, np.ndarray) and gltype is not None:
        return _p.glColorPointer(size, pointer, stride, gltype)  # C form
    if gltype is None:
        pointer = np.asarray(pointer)
        gltype = _p.gltype_of(pointer)
    else:
        pointer = np.asarray(pointer, dtype=_p.NPTYPE[gltype])
    _p.glColorPointer(size, gltype, stride, pointer)


def _xyz_pointer(rawfn, size, pointer, stride=0, ctype=None):
    if ctype is None:
        pointer = np.asarray(pointer)
        ctype = _p.gltype_of(pointer)
    else:
        pointer = np.asarray(pointer, dtype=_p.NPTYPE[ctype])
    rawfn(size, ctype, stride, pointer)


def glVertexPointer(size, pointer, stride=0, ctype=None):
    if _is_enum(pointer) and ctype is not None:  # C form (size, type, stride, ptr)
        return _p.glVertexPointer(size, pointer, stride, ctype)
    _xyz_pointer(_p.glVertexPointer, size, pointer, stride, ctype)


def glTexCoordPointer(size, pointer, stride=0, ctype=None):
    if _is_enum(pointer) and ctype is not None:
        return _p.glTexCoordPointer(size, pointer, stride, ctype)
    _xyz_pointer(_p.glTexCoordPointer, size, pointer, stride, ctype)


def glNormalPointer(pointer, stride=0, ctype=None):
    if _is_enum(pointer) and ctype is not None:  # C form (type, stride, ptr)
        return _p.glNormalPointer(pointer, stride, ctype)
    if ctype is None:
        pointer = np.asarray(pointer)
        ctype = _p.gltype_of(pointer)
    else:
        pointer = np.asarray(pointer, dtype=_p.NPTYPE[ctype])
    _p.glNormalPointer(ctype, stride, pointer)


def glDrawElements(mode, indices, *rest):
    if rest:  # C form (mode, count, type, indices)
        return _p.glDrawElements(mode, indices, rest[0], rest[1])
    a = np.ascontiguousarray(indices)
    _p.glDrawElements(mode, a.size, _p.gltype_of(a), a)


def glDrawPixels(width, *args):
    if len(args) == 4:  # C form (w, h, format, type, pixels)
        return _p.glDrawPixels(width, *args)
    format, pixels = args
    a = np.ascontiguousarray(pixels)
    npix = len(a)
    if npix % width:
        raise TypeError(npix, width, 'len( pixels) must be a multiple of "width"')
    _p.glDrawPixels(width, npix // width, format, _p.gltype_of(a), a)


def glReadPixels(x, y, width, *args):
    if len(args) == 2:  # opengltk form (x, y, width, format, pixels)
        format, pixels = args
        npix = len(pixels)
        if npix % width:
            raise TypeError(npix, width, 'len( pixels) must be a multiple of "width"')
        return _p.glReadPixels(x, y, width, npix // width, format,
                               _p.gltype_of(pixels), pixels)
    return _p.glReadPixels(x, y, width, *args)


def glTexImage2D(target, level, internalformat, width, height, border, format, *args):
    if len(args) == 2:  # C form (..., format, type, pixels)
        return _p.glTexImage2D(target, level, internalformat, width, height,
                               border, format, args[0], args[1])
    (pixels,) = args
    a = np.ascontiguousarray(pixels)
    _p.glTexImage2D(target, level, internalformat, width, height, border,
                    format, _p.gltype_of(a), a)


def glTexImage1D(target, level, internalformat, *args):
    if len(args) == 5:  # C form (width, border, format, type, pixels)
        return _p.glTexImage1D(target, level, internalformat, *args)
    border, format, pixels = args
    a = np.ascontiguousarray(pixels)
    _p.glTexImage1D(target, level, internalformat, len(a), border, format,
                    _p.gltype_of(a), a)


def glGetClipPlane(plane):
    return _p.flat(_p._GL.glGetClipPlane(plane), np.float64)


def glGetPolygonStipple(mask=None):
    return np.frombuffer(bytes(_p._GL.glGetPolygonStipple()), np.uint8)


def glPolygonStipple(mask):
    _p._GL.glPolygonStipple(np.ascontiguousarray(mask, dtype=np.uint8))


def glAreTexturesResident(textures):
    res = _p._GL.glAreTexturesResident(textures)
    return None if all(np.atleast_1d(res)) else np.asarray(res, np.uint8)


def glPrioritizeTextures(textures, priorities):
    _p._GL.glPrioritizeTextures(textures, priorities)
