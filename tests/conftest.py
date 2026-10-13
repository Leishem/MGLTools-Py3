"""Tests run against the installed mgltools package. Set MGL_USE_SRC=1 to
use the in-place build in src/ instead (python setup.py build_ext --inplace)."""
import os
import pathlib
import sys

SRC = pathlib.Path(__file__).resolve().parent.parent / "src"
if os.environ.get("MGL_USE_SRC"):
    sys.path.insert(0, str(SRC))
    os.environ["PYTHONPATH"] = str(SRC) + os.pathsep + os.environ.get("PYTHONPATH", "")
