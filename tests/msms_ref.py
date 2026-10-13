"""Reference: original MSMS (MGLTools 1.5.6, Python 2.5). Writes the surface
of a PDB to <out>.npz-free text files for comparison with mslib on py3.
Usage (pythonsh): msms_ref.py file.pdb outprefix [probe] [density]"""
import sys
from MolKit import Read
from mslib import MSMS

pdb, out = sys.argv[1], sys.argv[2]
probe = len(sys.argv) > 3 and float(sys.argv[3]) or 1.5
density = len(sys.argv) > 4 and float(sys.argv[4]) or 3.0
mol = Read(pdb)[0]
mol.defaultRadii()
atoms = mol.allAtoms
srf = MSMS(coords=atoms.coords, radii=atoms.radius)
srf.compute(probe_radius=probe, density=density)
srf.compute_ses_area()
srf.compute_numeric_area_vol()
c = srf.sesr.fst
vf, vi, tri = srf.getTriangles()
f = open(out + "_summary.txt", "w")
f.write("natoms %d\n" % len(atoms))
f.write("nvert %d\nntri %d\n" % (len(vf), len(tri)))
f.write("a_ses_area %.3f\na_sas_area %.3f\nn_ses_area %.3f\nn_ses_volume %.3f\n"
        % (c.a_ses_area, c.a_sas_area, c.n_ses_area, c.n_ses_volume))
f.close()
f = open(out + "_verts.txt", "w")
for i in range(len(vf)):
    f.write("%.4f %.4f %.4f %d %.4f %.4f %.4f\n" % (vf[i][0], vf[i][1], vf[i][2], vi[i][1], vf[i][3], vf[i][4], vf[i][5]))
f.close()
f = open(out + "_atomarea.txt", "w")
for i in range(srf.nbat):
    a = srf.get_atm(i)
    f.write("%.4f %.4f\n" % (a.get_ses_area(0), a.get_sas_area(0)))
f.close()
print "wrote", out
