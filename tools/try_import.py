"""Import each extension module and print the full error (undefined symbols etc.).

Usage: python tools/try_import.py [dotted.module ...]   (default: every built extension)
"""
import importlib
import pathlib
import sys

SRC = pathlib.Path(__file__).resolve().parent.parent / "src"
sys.path.insert(0, str(SRC))
mods = sys.argv[1:] or sorted(
    ".".join(p.relative_to(SRC).with_suffix("").with_suffix("").parts)
    for p in SRC.rglob("*.so"))
for m in mods:
    try:
        importlib.import_module(m)
        print("OK  ", m)
    except Exception as e:
        print("FAIL", m, "->", e)
