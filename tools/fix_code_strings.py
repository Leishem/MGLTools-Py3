"""Convert Python 2 code embedded in triple-quoted string literals.

MGLTools keeps executable code in strings: resource-file templates
(_dejavurc, _mglutilrc) and Vision node bodies (code = \"\"\"def doit...\"\"\").
2to3 never sees them. For each triple-quoted literal:
  1. skip unless the body parses with the Python 2 grammar (prose doesn't)
     and does NOT already compile as Python 3,
  2. run the fissix fixers on it, then the fix_py3_misc line rules,
  3. keep the result only if it compiles as Python 3.
Usage: python tools/fix_code_strings.py [paths...]
"""
import io
import pathlib
import sys
import textwrap
import tokenize
import warnings

from fissix import pygram, pytree
from fissix.pgen2 import driver
from fissix.refactor import RefactoringTool, get_fixers_from_package

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import fix_py3_misc  # noqa: E402

SRC = pathlib.Path(__file__).resolve().parent.parent / "src"
warnings.simplefilter("ignore")

_py2_driver = driver.Driver(pygram.python_grammar, convert=pytree.convert)
_tool = RefactoringTool(get_fixers_from_package("fissix.fixes"))


def compiles3(code):
    try:
        compile(code, "<s>", "exec")
        return True
    except (SyntaxError, ValueError):
        return False


def parses2(code):
    try:
        _py2_driver.parse_string(code)
        return True
    except Exception:
        return False


def convert(body):
    ind = len(body) - len(body.lstrip("\n"))
    lead = body[:ind]
    core = body[ind:]
    ded = textwrap.dedent(core)
    if not ded.strip() or compiles3(ded) or not parses2(ded + "\n"):
        return None
    try:
        new = str(_tool.refactor_string(ded + "\n", "<string>"))
    except Exception:
        return None
    new = "\n".join(fix_py3_misc.fix_modules(fix_py3_misc.fix_raise(l))
                    for l in new.split("\n"))
    if not compiles3(new):
        return None
    if not core.endswith("\n"):
        new = new[:-1] if new.endswith("\n") else new
    prefix = core[:len(core) - len(core.lstrip())] if core.strip() else ""
    if ded != core:  # re-indent to the original common indentation
        indent = core[:len(core) - len(core.lstrip(" "))]
        new = textwrap.indent(new, indent)
    del prefix
    return lead + new


def fix_file(path):
    text = path.read_text(encoding="utf-8")
    try:
        toks = list(tokenize.generate_tokens(io.StringIO(text).readline))
    except (tokenize.TokenError, SyntaxError, IndentationError):
        return 0
    lines = text.split("\n")
    offs = [0]
    for l in lines:
        offs.append(offs[-1] + len(l) + 1)
    edits = []
    for t in toks:
        if t.type != tokenize.STRING:
            continue
        s = t.string
        i = 0
        while i < len(s) and s[i] in "rRuU":
            i += 1
        q = s[i:i + 3]
        if q not in ('"""', "'''") or any(c in s[:i] for c in "bBfF"):
            continue
        body = s[i + 3:-3]
        if "\\" in body and s[:i].lower() != "r":
            continue  # escapes would change meaning; leave for manual review
        new = convert(body)
        if new is None or q in new:
            continue
        a = offs[t.start[0] - 1] + t.start[1]
        b = offs[t.end[0] - 1] + t.end[1]
        edits.append((a, b, s[:i + 3] + new + q))
    if not edits:
        return 0
    for a, b, rep in sorted(edits, reverse=True):
        text = text[:a] + rep + text[b:]
    if not compiles3(text):
        return 0
    path.write_text(text, encoding="utf-8", newline="")
    return len(edits)


def main():
    paths = [pathlib.Path(a) for a in sys.argv[1:]] or [SRC]
    files = [f for b in paths for f in ([b] if b.is_file() else b.rglob("*.py"))
             if "mglcompat" not in f.parts]
    total = 0
    for f in files:
        n = fix_file(f)
        if n:
            print(f"{n:3} {f.relative_to(SRC)}")
            total += n
    print(f"{total} code strings converted")


if __name__ == "__main__":
    main()
