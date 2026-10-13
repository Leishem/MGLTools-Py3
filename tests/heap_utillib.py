"""Exercise _utillib helpers that need no GL context, under MALLOC_CHECK_=3,
to catch heap corruption from SWIG typemaps."""
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "src"))
import numpy as np  # noqa: E402
from opengltk.extent import utillib  # noqa: E402

m = np.identity(4, 'f').ravel()
for i in range(2000):
    r = utillib.glCleanRotMat(m)
    if i == 0:
        print("glCleanRotMat ->", type(r), np.asarray(r).shape, np.asarray(r).dtype)
    _ = np.zeros(1000) + i  # churn the allocator

v = np.random.rand(30, 3)
f = np.array([[0, 1, 2], [3, 4, 5], [6, 7, 8]], 'i')
for name in ("triangleNormalsPerFace", "triangleNormalsPerVertex", "triangleNormalsBoth"):
    fn = getattr(utillib, name, None)
    if fn is None:
        print("missing", name)
        continue
    for i in range(500):
        try:
            r = fn(v, f)
        except Exception as e:
            print(name, "raised", e)
            break
        _ = np.zeros(1000) + i
    else:
        print(name, "ok", [np.asarray(x).shape for x in (r if isinstance(r, (list, tuple)) else [r])])

t = utillib.glTriangleNormals(v, f, 'PER_FACE') if hasattr(utillib, 'glTriangleNormals') else None
print("glTriangleNormals", None if t is None else np.asarray(t).shape)
print("done")
