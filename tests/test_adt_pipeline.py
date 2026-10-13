"""AutoDockTools preparation pipeline vs the original MGLTools 1.5.6.

Runs prepare_receptor4 / prepare_ligand4 / prepare_gpf4 / prepare_dpf42 on
the ADT tutorial inputs (HIV protease hsg1 + indinavir) and compares the
outputs with those of MGLTools 1.5.6 (tests/data/adt_ref_156, produced by
tools/save_golden.sh). REMARK lines carry dates/paths and are ignored.
"""
import os
import pathlib
import shutil
import subprocess
import sys

import pytest

import AutoDockTools  # noqa: E402  (installed package, or src/ via conftest)
import Pmv  # noqa: E402

ROOT = pathlib.Path(__file__).resolve().parent.parent
UTIL = pathlib.Path(AutoDockTools.__file__).parent / "Utilities24"
REF = ROOT / "tests" / "data" / "adt_ref_156"
DOC = pathlib.Path(Pmv.__file__).parent / "doc"

STEPS = [
    ("prepare_receptor4.py", ["-r", "hsg1.pdb", "-o", "hsg1.pdbqt"], "hsg1.pdbqt"),
    ("prepare_ligand4.py", ["-l", "ind.pdb", "-o", "ind.pdbqt"], "ind.pdbqt"),
    ("prepare_gpf4.py", ["-l", "ind.pdbqt", "-r", "hsg1.pdbqt", "-o", "hsg1.gpf"], "hsg1.gpf"),
    ("prepare_dpf42.py", ["-l", "ind.pdbqt", "-r", "hsg1.pdbqt", "-o", "ind_hsg1.dpf"], "ind_hsg1.dpf"),
]


def _lines(path):
    text = pathlib.Path(path).read_text(encoding="latin-1").replace("\r\n", "\n")
    return [l for l in text.split("\n") if not l.startswith("REMARK")]


@pytest.fixture(scope="module")
def workdir(tmp_path_factory):
    d = tmp_path_factory.mktemp("adt")
    shutil.copy(DOC / "hsg1.pdb", d)
    shutil.copy(DOC / "ind.pdb", d)
    env = dict(os.environ)
    for script, args, out in STEPS:
        r = subprocess.run([sys.executable, "-W", "ignore", str(UTIL / script)] + args,
                           cwd=d, env=env, capture_output=True, text=True, timeout=600)
        assert r.returncode == 0, f"{script} failed:\n{r.stdout}\n{r.stderr}"
    return d


@pytest.mark.parametrize("name", [s[2] for s in STEPS])
def test_matches_mgltools_156(workdir, name):
    new, ref = _lines(workdir / name), _lines(REF / name)
    assert new == ref, f"{name} differs from MGLTools 1.5.6 output"
