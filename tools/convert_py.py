"""Convert the MGLTools 1.5.6 Python 2 sources in src/ to Python 3.

Stage 1 (this script, text level):
  * decode each file (coding cookie, UTF-8, else Latin-1) and write UTF-8
  * expand tabs in indentation with Python 2's 8-column rule
  * numpy.oldnumeric / Numeric  -> mglcompat.oldnumeric
  * import string               -> from mglcompat import pystring as string
  * .tostring()                 -> .tobytes()
Stage 2: lib2to3 with the default fixers (needs Python <= 3.12, or fissix).

Usage: python3 tools/convert_py.py [--stage1-only] [paths...]
"""
import argparse
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
SRC = ROOT / "src"
SKIP_DIRS = {"mglcompat", "__pycache__"}

CODING_RX = re.compile(rb"^[ \t\f]*#.*?coding[:=][ \t]*([-\w.]+)")


def decode(raw):
    for line in raw.splitlines()[:2]:
        m = CODING_RX.match(line)
        if m:
            try:
                return raw.decode(m.group(1).decode("ascii"))
            except (LookupError, UnicodeDecodeError):
                break
    try:
        return raw.decode("utf-8")
    except UnicodeDecodeError:
        return raw.decode("latin-1")


def expand_indent(line):
    i = 0
    col = 0
    while i < len(line) and line[i] in " \t\f":
        if line[i] == "\t":
            col = (col // 8 + 1) * 8
        elif line[i] == " ":
            col += 1
        i += 1
    if "\t" not in line[:i]:
        return line
    return " " * col + line[i:]


IMPORT_LINE = re.compile(r"^(?P<ind>[ \t]*)import (?P<mods>[\w., ]+?)\s*(?P<tail>(?:;.*|#.*)?)$")


def fix_string_import(line):
    """'import os, string' -> 'import os; from mglcompat import pystring as string'"""
    m = IMPORT_LINE.match(line)
    if not m:
        return line
    mods = [x.strip() for x in m.group("mods").split(",")]
    if "string" not in mods:
        return line
    rest = [x for x in mods if x != "string"]
    parts = []
    if rest:
        parts.append("import " + ", ".join(rest))
    parts.append("from mglcompat import pystring as string")
    tail = m.group("tail")
    return m.group("ind") + "; ".join(parts) + ((" " + tail) if tail else "")


TEXT_RULES = [
    (re.compile(r"\bnumpy\.oldnumeric\b"), "mglcompat.oldnumeric"),
    (re.compile(r"^(\s*)import Numeric\s*$", re.M), r"\1import mglcompat.oldnumeric as Numeric"),
    (re.compile(r"^(\s*)import Numeric\s*,\s*", re.M), r"\1import mglcompat.oldnumeric as Numeric; import "),
    (re.compile(r"^(\s*)from Numeric import", re.M), r"\1from mglcompat.oldnumeric import"),
    (re.compile(r"\.tostring\(\)"), ".tobytes()"),
    (re.compile(r"^(\s*)from string import \*\s*$", re.M), r"\1from mglcompat.pystring import *"),
]


def stage1(path):
    raw = path.read_bytes()
    text = decode(raw)
    nl = "\r\n" if "\r\n" in text else "\n"
    lines = text.replace("\r\n", "\n").split("\n")
    out = []
    for i, line in enumerate(lines):
        if i < 2 and re.match(r"^[ \t\f]*#.*?coding[:=]", line):
            line = "# -*- coding: utf-8 -*-"
        line = expand_indent(line)
        line = fix_string_import(line)
        out.append(line)
    new = "\n".join(out)
    for rx, rep in TEXT_RULES:
        new = rx.sub(rep, new)
    new = new.replace("\n", nl)
    data = new.encode("utf-8")
    if data != raw:
        path.write_bytes(data)
        return True
    return False


def iter_py(paths):
    for base in paths:
        base = pathlib.Path(base)
        files = [base] if base.is_file() else base.rglob("*.py")
        for p in files:
            if not SKIP_DIRS.intersection(p.parts):
                yield p


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--stage1-only", action="store_true")
    ap.add_argument("--stage2-only", action="store_true")
    ap.add_argument("paths", nargs="*", default=[str(SRC)])
    a = ap.parse_args()
    files = list(iter_py(a.paths))
    if not a.stage2_only:
        n = sum(stage1(p) for p in files)
        print(f"stage1: {n}/{len(files)} files changed", file=sys.stderr)
    if a.stage1_only:
        return
    try:
        import fissix  # noqa: F401  (maintained lib2to3 fork)
        tool = [sys.executable, "-W", "ignore", "-m", "fissix"]
    except ImportError:
        tool = [sys.executable, "-W", "ignore", "-m", "lib2to3"]
    cmd = tool + ["-w", "-n", "-j", "16", "--no-diffs"] + [str(p) for p in files]
    r = subprocess.run(cmd, capture_output=True, text=True)
    errs = [l for l in r.stderr.splitlines()
            if "Can't parse" in l or "ParseError" in l or "Error" in l and "RefactoringTool: src" not in l]
    print(f"2to3: exit {r.returncode}, {len(errs)} problems", file=sys.stderr)
    for l in errs[:60]:
        print("  " + l, file=sys.stderr)


if __name__ == "__main__":
    main()
