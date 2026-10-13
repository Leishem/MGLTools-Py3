"""Import every module under src/ and report failures grouped by cause.

Usage: python tests/smoke_imports.py [package ...] [-v]
Run under a display (or xvfb-run) since many modules create Tk widgets.
"""
import collections
import importlib
import pathlib
import sys
import traceback
import warnings

SRC = pathlib.Path(__file__).resolve().parent.parent / "src"
sys.path.insert(0, str(SRC))
warnings.simplefilter("ignore")

SKIP_PARTS = {"Tests", "test", "tests", "Utilities24", "hostappInterface",
              "pdb2pqr", "VisionInterface"}

args = [a for a in sys.argv[1:] if not a.startswith("-")]
verbose = "-v" in sys.argv
pkgs = args or sorted(p.name for p in SRC.iterdir()
                      if (p / "__init__.py").exists())

mods = []
for pkg in pkgs:
    for f in sorted((SRC / pkg).rglob("*.py")):
        rel = f.relative_to(SRC).with_suffix("")
        if SKIP_PARTS.intersection(rel.parts):
            continue
        parts = list(rel.parts)
        if parts[-1] == "__init__":
            parts = parts[:-1]
        mods.append(".".join(parts))

ok = 0
causes = collections.Counter()
examples = {}
for m in mods:
    try:
        importlib.import_module(m)
        ok += 1
    except BaseException as e:  # noqa: BLE001  (SystemExit from scripts too)
        tb = traceback.extract_tb(e.__traceback__)
        where = f"{pathlib.Path(tb[-1].filename).name}:{tb[-1].lineno}" if tb else "?"
        key = f"{type(e).__name__}: {str(e)[:90]}"
        causes[key] += 1
        examples.setdefault(key, (m, where))
        if verbose:
            print(f"FAIL {m}: {key} @ {where}")

print(f"\n{ok}/{len(mods)} modules import")
for k, v in causes.most_common(40):
    m, where = examples[k]
    print(f"{v:4}  {k}\n        e.g. {m} @ {where}")
