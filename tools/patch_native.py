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
    for rx, rep in RULES:
        new = rx.sub(rep, new)
    if path.suffix == ".i":
        # %exception's $function placeholder was renamed $action (removed in
        # SWIG 4.3+)
        new = new.replace("$function", "$action")
        # t_output_helper was removed in SWIG 4.3; mgl_npcompat.h has a copy
        new = re.sub(r"\bt_output_helper\(", "mgl_t_output_helper(", new)
    if path.suffix == ".i" and "%module" in new and "python:cdefaultargs" not in new:
        # SWIG 4 writes C default args into the Python proxy (NULL becomes
        # 0, which typemaps then treat as data). SWIG 1.3 let C apply them.
        new = re.sub(r'^(%module[^\n]*\n)', r'\1%feature("python:cdefaultargs");\n',
                     new, count=1, flags=re.M)
    if path.suffix == ".i":
        # only outside %{ ... %} blocks, which hold verbatim C or Python
        parts = re.split(r"(%\{.*?%\})", new, flags=re.S)
        for i in range(0, len(parts), 2):
            for rx, rep in SWIG_RULES:
                parts[i] = rx.sub(rep, parts[i])
        new = "".join(parts)
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
