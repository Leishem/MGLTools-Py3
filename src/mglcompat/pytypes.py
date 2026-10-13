"""The Python 2 ``types`` module names on Python 3.

``from types import StringType`` fails on Python 3; fix_py3_misc.py rewrites
such imports to this module. Text types map to ``str`` because MGLTools
compares them against text read from files and widgets.
"""
import io as _io
from types import *  # noqa: F401,F403  (FunctionType, MethodType, ...)

NoneType = type(None)
TypeType = type
ObjectType = object
IntType = int
LongType = int
FloatType = float
BooleanType = bool
ComplexType = complex
StringType = str
UnicodeType = str
StringTypes = (str,)
BufferType = memoryview
TupleType = tuple
ListType = list
DictType = dict
DictionaryType = dict
ClassType = type
InstanceType = object
FileType = _io.IOBase
SliceType = slice
EllipsisType = type(Ellipsis)
XRangeType = range
