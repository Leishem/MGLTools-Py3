"""Byte-compile every .py under src/ and report syntax errors.

Usage: python tools/compile_check.py [-v]
"""
import collections
import pathlib
import sys

SRC = pathlib.Path(__file__).resolve().parent.parent / "src"
bad = collections.Counter()
lines = []
files = sorted(SRC.rglob("*.py"))
for p in files:
    try:
        compile(p.read_bytes(), str(p), "exec")
    except SyntaxError as e:
        key = f"{type(e).__name__}: {str(e.msg)[:70]}"
        bad[key] += 1
        lines.append(f"{p.relative_to(SRC)}:{e.lineno}: {key}")
print(f"{sum(bad.values())}/{len(files)} files fail to compile")
for k, v in bad.most_common():
    print(f"{v:5} {k}")
for l in lines if "-v" in sys.argv else lines[:50]:
    print(l)
sys.exit(1 if bad else 0)
