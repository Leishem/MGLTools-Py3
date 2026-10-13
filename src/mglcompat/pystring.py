"""The Python 2 ``string`` module API on Python 3.

The converter rewrites ``import string`` to ``from mglcompat import pystring
as string`` so module-function calls such as ``string.join(l, ',')`` keep
working without rewriting each call site.
"""
from string import (ascii_letters, ascii_lowercase, ascii_uppercase,  # noqa: F401
                    capwords, digits, hexdigits, octdigits, printable,
                    punctuation, whitespace, Formatter, Template)

letters = ascii_letters
lowercase = ascii_lowercase
uppercase = ascii_uppercase


def atoi(s, base=10):
    return int(s, base)


def atol(s, base=10):
    return int(s, base)


def atof(s):
    return float(s)


def split(s, sep=None, maxsplit=-1):
    return s.split(sep, maxsplit)


splitfields = split


def rsplit(s, sep=None, maxsplit=-1):
    return s.rsplit(sep, maxsplit)


def join(words, sep=' '):
    return sep.join(words)


joinfields = join


def strip(s, chars=None):
    return s.strip(chars)


def lstrip(s, chars=None):
    return s.lstrip(chars)


def rstrip(s, chars=None):
    return s.rstrip(chars)


def lower(s):
    return s.lower()


def upper(s):
    return s.upper()


def swapcase(s):
    return s.swapcase()


def capitalize(s):
    return s.capitalize()


def replace(s, old, new, maxreplace=-1):
    return s.replace(old, new, maxreplace)


def find(s, *args):
    return s.find(*args)


def rfind(s, *args):
    return s.rfind(*args)


def index(s, *args):
    return s.index(*args)


def rindex(s, *args):
    return s.rindex(*args)


def count(s, *args):
    return s.count(*args)


def zfill(x, width):
    if not isinstance(x, str):
        x = repr(x)
    return x.zfill(width)


def ljust(s, width, *args):
    return s.ljust(width, *args)


def rjust(s, width, *args):
    return s.rjust(width, *args)


def center(s, width, *args):
    return s.center(width, *args)


def expandtabs(s, tabsize=8):
    return s.expandtabs(tabsize)


def maketrans(fromstr, tostr):
    return str.maketrans(fromstr, tostr)


def translate(s, table, deletions=""):
    if deletions:
        table = dict(table) if isinstance(table, dict) else {}
        table.update({ord(c): None for c in deletions})
    if table is None:
        return s
    return s.translate(table)
