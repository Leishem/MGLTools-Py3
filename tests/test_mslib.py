"""mslib (open-source SES) vs the original MSMS 1.4.4 on HIV protease.

Reference values come from MSMS in MGLTools 1.5.6 (tools/save_golden.sh).
Tolerances reflect the grid method's measured accuracy, see
src/mslib/_ses.py and tests/sweep_msms.sh.
"""
import gzip
import pathlib
import sys

import numpy as np
import pytest

ROOT = pathlib.Path(__file__).resolve().parent.parent
REF = ROOT / "tests" / "data" / "adt_ref_156"


@pytest.fixture(scope="module")
def surface():
    from MolKit import Read
    from mslib import MSMS
    import Pmv, os
    mol = Read(os.path.join(os.path.dirname(Pmv.__file__), "doc", "hsg1.pdb"))[0]
    mol.defaultRadii()
    srf = MSMS(coords=mol.allAtoms.coords, radii=mol.allAtoms.radius)
    assert srf.compute(probe_radius=1.5, density=3.0) == 1
    srf.compute_ses_area()
    srf.compute_numeric_area_vol()
    return srf


@pytest.fixture(scope="module")
def ref():
    summary = dict(l.split() for l in open(REF / "msms_hsg1_summary.txt"))
    with gzip.open(REF / "msms_hsg1_verts.txt.gz", "rt") as f:
        verts = np.loadtxt(f)
    return summary, verts


def test_areas_and_volume(surface, ref):
    summary, _ = ref
    c = surface.sesr.fst
    assert abs(c.a_ses_area / float(summary["a_ses_area"]) - 1) < 0.06
    assert abs(c.n_ses_volume / float(summary["n_ses_volume"]) - 1) < 0.04
    assert abs(c.a_sas_area / float(summary["a_sas_area"]) - 1) < 0.06


def test_vertices_on_msms_surface(surface, ref):
    from scipy.spatial import cKDTree
    _, rv = ref
    vf, vi, tri = surface.getTriangles()
    _, near = cKDTree(rv[:, :3]).query(vf[:, :3])
    off = np.einsum('ij,ij->i', vf[:, :3] - rv[near, :3], rv[near, 4:7])
    assert abs(off.mean()) < 0.05, off.mean()
    assert off.std() < 0.2, off.std()


def test_get_triangles_api(surface):
    vf, vi, tri = surface.getTriangles()
    assert vf.shape[1] == 8 and vi.shape[1] == 3 and tri.shape[1] == 5
    assert vi[:, 1].min() >= 1 and vi[:, 1].max() <= surface.nbat
    sub_vf, sub_vi, sub_tri = surface.getTriangles([0, 1, 2, 3, 4], keepOriginalIndices=1)
    assert len(sub_vf) == len(vf) and 0 < len(sub_tri) < len(tri)
    cvf, cvi, ctri = surface.getTriangles([0, 1, 2, 3, 4])
    assert ctri[:, :3].max() < len(cvf)
