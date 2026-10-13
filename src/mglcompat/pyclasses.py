"""Mixins restoring Python 2 special-method behaviour that MGLTools relies on.

SliceMixin: Python 3 never calls __getslice__/__setslice__/__delslice__;
    obj[i:j] goes to __getitem__ with a slice. This mixin forwards simple
    slices to the class's Python 2 slice methods with the same index
    normalisation Python 2 applied (negative indices wrapped, open end ->
    a value past the end), and everything else to the next class in the MRO.

CmpMixin: Python 3 ignores __cmp__. This mixin derives the rich comparisons
    from it. Classes that define __eq__/__ne__ themselves keep them, and
    instances stay hashable by identity as they were in Python 2.

Put the mixin first in the bases: class AtomSet(SliceMixin, UserList).
"""
import sys


def _py2_bounds(n, s):
    """(i, j) as Python 2 passed them to __getslice__."""
    i = 0 if s.start is None else s.start
    j = sys.maxsize if s.stop is None else s.stop
    if i < 0:
        i = max(0, i + n)
    if j < 0:
        j = max(0, j + n)
    return i, j


class SliceMixin:
    def __getitem__(self, i):
        if isinstance(i, slice) and i.step in (None, 1) and hasattr(self, '__getslice__'):
            return self.__getslice__(*_py2_bounds(len(self), i))
        return super().__getitem__(i)

    def __setitem__(self, i, value):
        if isinstance(i, slice) and i.step in (None, 1) and hasattr(self, '__setslice__'):
            return self.__setslice__(*_py2_bounds(len(self), i), value)
        return super().__setitem__(i, value)

    def __delitem__(self, i):
        if isinstance(i, slice) and i.step in (None, 1) and hasattr(self, '__delslice__'):
            return self.__delslice__(*_py2_bounds(len(self), i))
        return super().__delitem__(i)


class CmpMixin:
    __hash__ = object.__hash__

    def __eq__(self, other):
        return self.__cmp__(other) == 0

    def __ne__(self, other):
        return self.__cmp__(other) != 0

    def __lt__(self, other):
        return self.__cmp__(other) < 0

    def __le__(self, other):
        return self.__cmp__(other) <= 0

    def __gt__(self, other):
        return self.__cmp__(other) > 0

    def __ge__(self, other):
        return self.__cmp__(other) >= 0


class OldStyleProperty:
    """property() as Python 2 old-style classes saw it.

    On an old-style class only the getter ran: assignment stored a plain
    instance attribute (no setter call), and later reads returned that
    attribute. A getter-only, non-data descriptor reproduces this. Use it for
    properties defined on classes that were old-style in MGLTools 1.5.6.
    """

    def __init__(self, fget, fset=None, fdel=None, doc=None):
        self.fget = fget
        self.__doc__ = doc or getattr(fget, "__doc__", None)

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        return self.fget(obj)


def cmp(a, b):
    """Python 2 builtin cmp()."""
    return (a > b) - (a < b)
