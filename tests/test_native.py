"""Every native extension imports and its basic entry points work."""
import importlib
import pathlib
import sys

import numpy as np
import pytest


MODULES = [
    "bhtree._bhtreelib", "geomutils._efitlib", "geomutils._geomalgorithms",
    "gle._gle", "pyglf._glf", "sff._prmlib", "QSlimLib._qslimlib",
    "opengltk.extent._utillib", "UTpackages.UTblur._blur",
    "UTpackages.UTisocontour._isocontour", "UTpackages.UTmesh._lbiemesher",
    "UTpackages.UTmolderivatives._molderivatives", "UTpackages.UTsdf._utsdf",
    "UTpackages.UTvolrend._UTVolumeLibrary",
]
if sys.platform != "win32":
    MODULES.append("cMolKit._xtcparser")


@pytest.mark.parametrize("mod", MODULES)
def test_import(mod):
    importlib.import_module(mod)


def test_bhtree_close_points():
    from bhtree import bhtreelib
    pts = np.array([[0, 0, 0], [1, 0, 0], [5, 5, 5], [0.5, 0, 0]], 'f')
    t = bhtreelib.BHtree(pts, None, 10)
    res = np.zeros(10, 'i'); d = np.zeros(10, 'f')
    nb = t.closePointsDist2((0., 0., 0.), 1.5, res, d)
    assert nb == 3 and sorted(res[:nb]) == [0, 1, 3]


def test_utillib_no_heap_corruption():
    from opengltk.extent import utillib
    m = np.identity(4, 'f').ravel()
    for _ in range(500):
        r = utillib.glCleanRotMat(m)
    assert np.asarray(r).shape == (4, 4)
    v = np.random.rand(30, 3); f = np.array([[0, 1, 2], [3, 4, 5]], 'i')
    for _ in range(200):
        n = utillib.triangleNormalsPerVertex(v, f)
    assert np.asarray(n).shape == (30, 3)


def test_isocontour_sphere():
    from UTpackages.UTisocontour import isocontour
    isocontour.setVerboseLevel(0)
    h, o = 0.5, np.array([-5., -5., -5.], 'f')
    ax = o[0] + h * np.arange(21)
    X, Y, Z = np.meshgrid(ax, ax, ax, indexing="ij")
    field = (3.0 - np.sqrt(X**2 + Y**2 + Z**2)).astype('f')
    data = np.ascontiguousarray(np.reshape(np.transpose(field), (1, 1) + field.shape), 'f')
    ds = isocontour.newDatasetRegFloat3D(data, o, np.array([h, h, h], 'f'))
    iso = isocontour.getContour3d(ds, 0, 0, 0.0, isocontour.NO_COLOR_VARIABLE)
    v = np.zeros((iso.nvert, 3), 'f'); nrm = np.zeros((iso.nvert, 3), 'f')
    c = np.zeros(iso.nvert, 'f'); t = np.zeros((iso.ntri, 3), 'i')
    isocontour.getContour3dData(iso, v, nrm, c, t, 0)
    assert abs(np.linalg.norm(v, axis=1).mean() - 3.0) < 0.02
