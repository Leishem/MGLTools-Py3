"""Second-pass Python 3 fixes that 2to3 does not cover.

  raise 'msg'            -> raise RuntimeError('msg')
  raise 'msg', value     -> raise RuntimeError('msg', value)
  x is 'lit' / x is 0    -> x == 'lit' / x == 0  (and 'is not' -> '!=')
  self.__debug__         -> self._mgl_debug     (assignment is illegal in py3)
  async (as a name)      -> async_

The raise rule is line-based (single-line forms only); the others work on
tokens so strings and comments are never touched.
Usage: python tools/fix_py3_misc.py [paths...]
"""
import io
import pathlib
import re
import sys
import tokenize

SRC = pathlib.Path(__file__).resolve().parent.parent / "src"

STR = r"""(?:'[^'\\\n]*(?:\\.[^'\\\n]*)*'|"[^"\\\n]*(?:\\.[^"\\\n]*)*")"""
RAISE_RX = re.compile(r"^(\s*|.*?:\s*)raise\s+(" + STR + r")\s*(?:,\s*(.+?))?\s*(#.*)?$")
TYPES_IMPORT_RX = re.compile(r"^(\s*)from\s+types\s+import\s+(.+)$")
PY2_TYPE_NAMES = {
    "NoneType", "TypeType", "ObjectType", "IntType", "LongType", "FloatType",
    "BooleanType", "ComplexType", "StringType", "UnicodeType", "StringTypes",
    "BufferType", "TupleType", "ListType", "DictType", "DictionaryType",
    "ClassType", "InstanceType", "FileType", "SliceType", "XRangeType", "*",
}


def fix_raise(line):
    m = RAISE_RX.match(line)
    if not m:
        return line
    ind, msg, val, comment = m.groups()
    args = msg + (", " + val if val else "")
    return f"{ind}raise RuntimeError({args})" + (f"  {comment}" if comment else "")


STRING_FUNCS = {"atoi", "atol", "atof", "split", "splitfields", "rsplit", "join",
                "joinfields", "strip", "lstrip", "rstrip", "lower", "upper",
                "swapcase", "capitalize", "replace", "find", "rfind", "index",
                "rindex", "count", "zfill", "ljust", "rjust", "center",
                "expandtabs", "maketrans", "translate", "letters", "lowercase",
                "uppercase"}
FROM_STRING_RX = re.compile(r"^(\s*)from\s+string\s+import\s+(.+)$")
MODULE_SWAPS = [
    # (regex on a whole line, replacement)
    (re.compile(r"^(\s*)import user\s*$"), r"\1from mglcompat import pyuser as user"),
    (re.compile(r"^(\s*)import UserDict\s*$"), r"\1import collections as UserDict"),
    (re.compile(r"^(\s*)import UserList\s*$"), r"\1import collections as UserList"),
    (re.compile(r"^(\s*)from UserDict import"), r"\1from collections import"),
    (re.compile(r"^(\s*)from UserList import"), r"\1from collections import"),
    (re.compile(r"^(\s*)import Canvas\s*$"), r"\1from mglcompat import tkCanvas as Canvas"),
    (re.compile(r"^(\s*)from Canvas import"), r"\1from mglcompat.tkCanvas import"),
    (re.compile(r"\binspect\.getargspec\b"), "inspect.getfullargspec"),
    (re.compile(r"^(\s*)from inspect import getargspec\b"),
     r"\1from inspect import getfullargspec as getargspec"),
]


PIL_MODULES = {"Image", "ImageTk", "ImageDraw", "ImageFont", "ImageOps",
               "ImageEnhance", "ImageChops", "ImageFilter", "ImageGrab",
               "ImageFile", "ImageColor", "ImagePalette", "ImageStat",
               "ImageSequence", "ImageMath", "PngImagePlugin", "GifImagePlugin"}
IMPORT_STMT_RX = re.compile(r"^(?P<ind>\s*)import\s+(?P<mods>[\w., ]+?)\s*(?P<tail>(?:#.*)?)$")
FROM_PIL_RX = re.compile(r"^(\s*)from\s+(" + "|".join(sorted(PIL_MODULES)) + r")\s+import\b")


def split_import(line):
    """'import Image, os, user' -> separate statements for moved modules."""
    m = IMPORT_STMT_RX.match(line)
    if not m:
        return line
    keep, moved = [], []
    for item in (x.strip() for x in m.group("mods").split(",")):
        name, _, alias = item.partition(" as ")
        name, alias = name.strip(), alias.strip()
        if name in PIL_MODULES:
            moved.append(f"from PIL import {name}" + (f" as {alias}" if alias else ""))
        elif name == "user":
            moved.append("from mglcompat import pyuser as " + (alias or "user"))
        else:
            keep.append(item)
    if not moved:
        return line
    stmts = (["import " + ", ".join(keep)] if keep else []) + moved
    tail = m.group("tail")
    return m.group("ind") + "; ".join(stmts) + (("  " + tail) if tail else "")


def fix_modules(line):
    for rx, rep in MODULE_SWAPS:
        line = rx.sub(rep, line)
    line = split_import(line)
    line = FROM_PIL_RX.sub(r"\1from PIL.\2 import", line)
    m = FROM_STRING_RX.match(line)
    if m:
        names = {n.strip().split(" as ")[0] for n in m.group(2).split("#")[0].strip("() ").split(",")}
        if names & STRING_FUNCS:
            line = f"{m.group(1)}from mglcompat.pystring import {m.group(2)}"
    return line


def fix_types_import(line):
    m = TYPES_IMPORT_RX.match(line)
    if not m:
        return line
    names = {n.strip().split(" as ")[0] for n in m.group(2).split("#")[0].strip("() ").split(",")}
    if names & PY2_TYPE_NAMES:
        return f"{m.group(1)}from mglcompat.pytypes import {m.group(2)}"
    return line


def token_edits(text):
    """Return [(row, col_start, col_end, replacement)] for code tokens."""
    try:
        toks = list(tokenize.generate_tokens(io.StringIO(text).readline))
    except (tokenize.TokenError, SyntaxError, IndentationError):
        return None
    edits = []
    code = [t for t in toks if t.type not in (tokenize.NL, tokenize.COMMENT)]
    for i, t in enumerate(code):
        if t.type == tokenize.NAME and t.string == "async":
            edits.append((t.start[0], t.start[1], t.end[1], "async_"))
        elif t.type == tokenize.NAME and t.string == "__debug__" and i >= 2 \
                and code[i - 1].string == "." and code[i - 2].string == "self":
            edits.append((t.start[0], t.start[1], t.end[1], "_mgl_debug"))
        elif t.type == tokenize.NAME and t.string == "is":
            j = i + 1
            neg = j < len(code) and code[j].type == tokenize.NAME and code[j].string == "not"
            if neg:
                j += 1
            if j < len(code) and code[j].type == tokenize.OP and code[j].string == "-":
                j += 1
            if j < len(code) and code[j].type in (tokenize.STRING, tokenize.NUMBER):
                nxt = code[j + 1] if j + 1 < len(code) else None
                if nxt is None or nxt.string not in (".", "[", "("):
                    end = code[i + 1].end if neg else t.end
                    if end[0] == t.start[0]:
                        edits.append((t.start[0], t.start[1], end[1], "!=" if neg else "=="))
    return edits


def fix_file(p):
    text = p.read_text(encoding="utf-8")
    lines = text.split("\n")
    lines = [fix_modules(fix_types_import(fix_raise(l))) for l in lines]
    text2 = "\n".join(lines)
    edits = token_edits(text2)
    if edits:
        lines = text2.split("\n")
        for row, c0, c1, rep in sorted(edits, key=lambda e: (e[0], -e[1])):
            l = lines[row - 1]
            lines[row - 1] = l[:c0] + rep + l[c1:]
        text2 = "\n".join(lines)
    if text2 != text:
        p.write_text(text2, encoding="utf-8", newline="")
        return True
    return False


def main():
    paths = [pathlib.Path(a) for a in sys.argv[1:]] or [SRC]
    files = [f for b in paths for f in ([b] if b.is_file() else b.rglob("*.py"))
             if "mglcompat" not in f.parts]
    n = sum(fix_file(f) for f in files)
    print(f"{n}/{len(files)} files changed")


if __name__ == "__main__":
    main()
