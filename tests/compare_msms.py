"""Compare the py3 mslib surface with original MSMS output (tests/msms_ref.py).

Usage: python tests/compare_msms.py file.pdb refprefix [probe] [density]
Reports areas/volume differences and the distance from each new vertex to
the nearest MSMS vertex (MSMS vertices lie on the analytical SES).
"""
import pathlib
import sys
import time

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "src"))
import numpy as np  # noqa: E402
from scipy.spatial import cKDTree  # noqa: E402
from MolKit import Read  # noqa: E402
from mslib import MSMS  # noqa: E402

pdb, ref = sys.argv[1], sys.argv[2]
probe = float(sys.argv[3]) if len(sys.argv) > 3 else 1.5
density = float(sys.argv[4]) if len(sys.argv) > 4 else 3.0

refsum = dict(l.split() for l in open(ref + "_summary.txt"))
refv = np.loadtxt(ref + "_verts.txt")
refa = np.loadtxt(ref + "_atomarea.txt")

mol = Read(pdb)[0]
mol.defaultRadii()
atoms = mol.allAtoms
t0 = time.time()
srf = MSMS(coords=atoms.coords, radii=atoms.radius)
assert srf.compute(probe_radius=probe, density=density) == 1
t1 = time.time()
srf.compute_ses_area()
srf.compute_numeric_area_vol()
t2 = time.time()
c = srf.sesr.fst
vf, vi, tri = srf.getTriangles()


def pct(new, old):
    return 100.0 * (new - old) / old if old else float("nan")


print("atoms %d   surface %.2fs   areas %.2fs" % (len(atoms), t1 - t0, t2 - t1))
print("%-14s %12s %12s %8s" % ("", "MSMS", "mslib", "diff%"))
for key, new in (("nvert", len(vf)), ("ntri", len(tri)),
                 ("a_ses_area", c.a_ses_area), ("a_sas_area", c.a_sas_area),
                 ("n_ses_volume", c.n_ses_volume)):
    old = float(refsum[key])
    print("%-14s %12.1f %12.1f %8.2f" % (key, old, new, pct(new, old)))

d, near = cKDTree(refv[:, :3]).query(vf[:, :3])
print("vertex -> nearest MSMS vertex: mean %.3f  95%% %.3f  max %.3f A"
      % (d.mean(), np.percentile(d, 95), d.max()))
if refv.shape[1] >= 7:
    # offset along the MSMS normal (outward): >0 means the new surface is
    # outside the analytical SES
    off = np.einsum('ij,ij->i', vf[:, :3] - refv[near, :3], refv[near, 4:7])
    print("signed offset along MSMS normal: mean %+.3f  sd %.3f  |off| 95%% %.3f A"
          % (off.mean(), off.std(), np.percentile(np.abs(off), 95)))
d2, idx = cKDTree(vf[:, :3]).query(refv[:, :3])
print("MSMS vertex -> nearest new vertex: mean %.3f  95%% %.3f  max %.3f A"
      % (d2.mean(), np.percentile(d2, 95), d2.max()))
same_atom = np.mean(vi[idx, 1] == refv[:, 3].astype(int))
print("closest-atom agreement at matched vertices: %.1f%%" % (100 * same_atom))
ses_new = np.array([srf.get_atm(i).get_ses_area(0) for i in range(srf.nbat)])
sas_new = np.array([srf.get_atm(i).get_sas_area(0) for i in range(srf.nbat)])
print("per-atom SAS corr %.4f  mean |diff| %.2f A^2" %
      (np.corrcoef(sas_new, refa[:, 1])[0, 1], np.abs(sas_new - refa[:, 1]).mean()))
print("per-atom SES corr %.4f  mean |diff| %.2f A^2" %
      (np.corrcoef(ses_new, refa[:, 0])[0, 1], np.abs(ses_new - refa[:, 0]).mean()))
