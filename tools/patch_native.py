"""Patch the MGLTools 1.5.4 SWIG/C sources under native/ for SWIG 4,
Python 3 and NumPy 2. Idempotent: safe to run repeatedly.

Usage: python tools/patch_native.py
"""
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent / "native"
EXTS = {".i", ".h", ".c", ".cpp", ".cxx", ".cc", ".C"}

RULES = [
    # numpy headers -> compat header (which includes numpy/arrayobject.h)
    (re.compile(r'#include\s*[<"](?:numpy/)?arrayobject\.h[>"]'),
     '#include "mgl_npcompat.h"'),
    (re.compile(r'#include\s*[<"]Numeric/arrayobject\.h[>"]'),
     '#include "mgl_npcompat.h"'),
    (re.compile(r'#include\s*[<"]numpy/oldnumeric\.h[>"]'),
     '#include "mgl_npcompat.h"'),
    # SWIG 1.1 language-qualified typemaps: %typemap(python, in) -> %typemap(in)
    (re.compile(r'%typemap\(\s*python\s*,\s*'), '%typemap('),
    # Windows Vista ownership hack: keep only the portable branch
    (re.compile(
        r'#ifdef _MSC_VER\s*\n\s*switch\s*\(\s*WinVerMajor\(\)\s*\)\s*\{.*?\}\s*\n\s*#else\s*\n(.*?)#endif',
        re.S), r'\1'),
    # x->flags |= NPY_OWNDATA / OWN_DATA  ->  MGL_SET_OWNDATA(x)
    (re.compile(r'(\(?\s*\(?PyArrayObject\s*\*\)?\s*\w+\)?|\b\w+)\s*->\s*flags\s*\|=\s*(?:NPY_OWNDATA|OWN_DATA|NPY_ARRAY_OWNDATA)\s*;'),
     r'MGL_SET_OWNDATA(\1);'),
    # PyCObject (py2) -> PyCapsule
    (re.compile(r'\bPyCObject_FromVoidPtr\(([^,]+),\s*NULL\)'), r'PyCapsule_New(\1, NULL, NULL)'),
    (re.compile(r'\bPyCObject_AsVoidPtr\(([^)]+)\)'), r'PyCapsule_GetPointer(\1, NULL)'),
    (re.compile(r'\bPyCObject_Check\b'), 'PyCapsule_CheckExact'),
]

# SWIG-only rules (.i files): '#comment' lines that SWIG 4 parses as directives
SWIG_RULES = [
    (re.compile(r'^#(?!\s*(?:include|define|if|ifdef|ifndef|else|elif|endif|undef|error|warning|pragma|line)\b)',
                re.M), '//'),
]


def patch(path):
    try:
        text = path.read_text(encoding="latin-1")
    except OSError as e:
        print("skip", path, e)
        return False
    new = text
    for rx, rep in RULES + (SWIG_RULES if path.suffix == ".i" else []):
        new = rx.sub(rep, new)
    if new != text:
        path.write_text(new, encoding="latin-1", newline="")
        return True
    return False


def main():
    changed = [p for p in ROOT.rglob("*") if p.suffix in EXTS and p.is_file()
               and "common" not in p.parts and patch(p)]
    for p in changed:
        print("patched", p.relative_to(ROOT))
    print(len(changed), "files patched", file=sys.stderr)


if __name__ == "__main__":
    main()
