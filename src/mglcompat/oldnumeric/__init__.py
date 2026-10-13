"""Replacement for ``numpy.oldnumeric`` on modern NumPy.

MGLTools 1.5.6 was written against numpy.oldnumeric, which kept the
Numeric API: typecode strings, ``sum``/``take`` defaulting to axis 0,
``fromstring`` for binary data, and Numeric-style type names. Only the
behaviour MGLTools relies on is reproduced here.
"""
import numpy as _np
from numpy import *  # noqa: F401,F403  (Numeric re-exported most of numpy)
from numpy import ndarray, newaxis, pi, e  # noqa: F401

# --- type names (numpy/oldnumeric/precision.py) ------------------------------
Character = 'c'
Int0 = 'b'          # oldnumeric mapped 'b' to unsigned byte, see _oldtype
Int8 = '1'
Int16 = 's'
Int32 = 'i'
Int64 = 'q'
Int = 'l'
UnsignedInt8 = 'b'
UInt8 = 'b'
UnsignedInt16 = 'w'
UInt16 = 'w'
UnsignedInt32 = 'u'
UInt32 = 'u'
UInt = 'u'
Float0 = 'f'
Float8 = 'f'
Float16 = 'f'
Float32 = 'f'
Float64 = 'd'
Float = 'd'
Complex0 = 'F'
Complex8 = 'F'
Complex16 = 'F'
Complex32 = 'F'
Complex64 = 'D'
Complex = 'D'
PyObject = 'O'

ArrayType = ndarray
NewAxis = None
typecodes = {'Character': 'c', 'Integer': '1sil', 'UnsignedInteger': 'bwu',
             'Float': 'fd', 'Complex': 'FD'}

# Numeric typecode characters whose meaning differs from numpy's
_oldtype = {
    '1': _np.int8, 's': _np.int16, 'w': _np.uint16, 'u': _np.uint32,
    'b': _np.uint8, 'c': 'S1', 'l': _np.int_, 'i': _np.intc,
}


def convtypecode(typecode, dtype=None):
    if dtype is not None:
        return _np.dtype(dtype)
    if typecode is None:
        return None
    if isinstance(typecode, str) and typecode in _oldtype:
        return _np.dtype(_oldtype[typecode])
    return _np.dtype(typecode)


def _tc(typecode, dtype, default=None):
    t = convtypecode(typecode, dtype)
    return default if t is None else t


# --- constructors ------------------------------------------------------------
def array(sequence, typecode=None, copy=1, savespace=0, dtype=None):
    return _np.array(sequence, dtype=_tc(typecode, dtype), copy=bool(copy))


def asarray(a, typecode=None, dtype=None):
    return _np.asarray(a, dtype=_tc(typecode, dtype))


def zeros(shape, typecode='l', savespace=0, dtype=None):
    return _np.zeros(shape, dtype=_tc(typecode, dtype, _np.int_))


def ones(shape, typecode='l', savespace=0, dtype=None):
    return _np.ones(shape, dtype=_tc(typecode, dtype, _np.int_))


def empty(shape, typecode='l', dtype=None):
    return _np.empty(shape, dtype=_tc(typecode, dtype, _np.int_))


def identity(n, typecode='l', dtype=None):
    return _np.identity(n, dtype=_tc(typecode, dtype, _np.int_))


def arange(start, stop=None, step=1, typecode=None, dtype=None):
    if stop is None:
        return _np.arange(start, step=step, dtype=_tc(typecode, dtype))
    return _np.arange(start, stop, step, dtype=_tc(typecode, dtype))


arrayrange = arange


def fromstring(string, typecode='l', count=-1, dtype=None):
    if isinstance(string, str):
        string = string.encode('latin-1')
    return _np.frombuffer(string, dtype=_tc(typecode, dtype, _np.int_),
                          count=count).copy()


def fromfunction(function, dimensions, typecode=None, dtype=float):
    return _np.fromfunction(function, dimensions,
                            dtype=_tc(typecode, None, dtype))


# --- functions whose defaults differ from numpy -------------------------------
def sum(x, axis=0, dtype=None):
    return _np.sum(x, axis=axis, dtype=dtype)


def product(x, axis=0, dtype=None):
    return _np.prod(x, axis=axis, dtype=dtype)


def cumsum(x, axis=0):
    return _np.cumsum(x, axis=axis)


def cumproduct(x, axis=0):
    return _np.cumprod(x, axis=axis)


def take(a, indices, axis=0):
    return _np.take(a, indices, axis=axis)


def repeat(a, repeats, axis=0):
    return _np.repeat(a, repeats, axis=axis)


def argmax(x, axis=-1):
    return _np.argmax(x, axis=axis)


def argmin(x, axis=-1):
    return _np.argmin(x, axis=axis)


def argsort(a, axis=-1):
    return _np.argsort(a, axis=axis)


def sort(a, axis=-1):
    return _np.sort(a, axis=axis)


def alltrue(x, axis=0):
    return _np.all(x, axis=axis)


def sometrue(x, axis=0):
    return _np.any(x, axis=axis)


def average(a, axis=0, weights=None, returned=False):
    return _np.average(a, axis=axis, weights=weights, returned=returned)


def nonzero(a):
    # Numeric returned a flat index array for 1-D input
    return _np.flatnonzero(a)


def resize(a, shape):
    return _np.resize(a, shape)


def reshape(a, shape):
    return _np.reshape(a, shape)


def compress(condition, a, axis=-1):
    return _np.compress(condition, a, axis=axis)


def cross_product(a, b, axis1=-1, axis2=-1):
    return _np.cross(a, b, axis1, axis2)


def outerproduct(a, b):
    return _np.outer(a, b)


def innerproduct(a, b):
    return _np.inner(a, b)


matrixmultiply = _np.dot
dot = _np.dot
absolute = _np.absolute
around = _np.round


def rank(a):
    return _np.ndim(a)


def size(a, axis=None):
    return _np.size(a, axis)


def shape(a):
    return _np.shape(a)


def ravel(m):
    return _np.ravel(m)


def choose(a, choices):
    return _np.choose(a, choices)


def putmask(a, mask, values):
    return _np.putmask(a, mask, values)


def array2string(a, max_line_width=None, precision=None, suppress_small=None,
                 separator=' ', array_output=0):
    return _np.array2string(a, max_line_width=max_line_width,
                            precision=precision, suppress_small=suppress_small,
                            separator=separator)


def asbuffer(a):
    return _np.ascontiguousarray(a).tobytes()


def typecode(a):
    return _np.asarray(a).dtype.char
