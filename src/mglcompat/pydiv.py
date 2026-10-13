"""Python 2 ``/`` semantics for code converted by tools/convert_division.sh.

In Python 2 (without ``from __future__ import division``), ``a / b`` floored
when both operands were integers -- including NumPy integer scalars and
arrays -- and was true division otherwise. Every ``/`` whose operand types
could not be proven float was rewritten to ``old_div(a, b)``.
"""
import numbers

import numpy as _np


def _is_int(x):
    if isinstance(x, bool):
        return True
    if isinstance(x, numbers.Integral):
        return True
    if isinstance(x, _np.ndarray):
        return x.dtype.kind in "iub"
    return isinstance(x, _np.integer)


def old_div(a, b):
    if _is_int(a) and _is_int(b):
        return a // b
    return a / b
