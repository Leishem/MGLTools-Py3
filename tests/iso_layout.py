"""Check UTisocontour's array layout with an analytic sphere field."""
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "src"))
import numpy as np  # noqa: E402
from UTpackages.UTisocontour import isocontour  # noqa: E402

isocontour.setVerboseLevel(0)
nx, ny, nz = 30, 40, 50
h = 0.5
origin = np.array([-5.0, -8.0, -12.0], 'f')
center = np.array([1.0, 2.0, 3.0])
x = origin[0] + h * np.arange(nx)
y = origin[1] + h * np.arange(ny)
z = origin[2] + h * np.arange(nz)
X, Y, Z = np.meshgrid(x, y, z, indexing="ij")
field = (4.0 - np.sqrt((X - center[0])**2 + (Y - center[1])**2 + (Z - center[2])**2)).astype('f')

data = np.ascontiguousarray(np.reshape(np.transpose(field), (1, 1) + field.shape), 'f')
ds = isocontour.newDatasetRegFloat3D(data, origin, np.array([h, h, h], 'f'))
iso = isocontour.getContour3d(ds, 0, 0, 0.0, isocontour.NO_COLOR_VARIABLE)
v = np.zeros((iso.nvert, 3), 'f'); n = np.zeros((iso.nvert, 3), 'f')
c = np.zeros(iso.nvert, 'f'); t = np.zeros((iso.ntri, 3), 'i')
isocontour.getContour3dData(iso, v, n, c, t, 0)
r = np.linalg.norm(v - center, axis=1)
print("nvert", iso.nvert, "ntri", iso.ntri, "radius mean %.3f min %.3f max %.3f" % (r.mean(), r.min(), r.max()))
out = (v - center) / r[:, None]
print("normal . outward mean %.3f" % np.mean(np.sum(n * out, axis=1)))
f = np.cross(v[t[:, 1]] - v[t[:, 0]], v[t[:, 2]] - v[t[:, 0]])
cen = v[t].mean(axis=1) - center
print("face winding . outward mean %.3f" % np.mean(np.sign(np.sum(f * cen, axis=1))))
