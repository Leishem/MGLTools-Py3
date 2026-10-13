"""Grid-based solvent excluded surface (SES) -- open-source stand-in for MSMS.

MSMS (M. Sanner) computes the SES analytically but is distributed only as
closed binaries for 2000s-era platforms. This module computes the same
surface numerically:

1. Solvent accessible region: grid points outside every sphere of radius
   r_i + probe. Only the region connected to the grid border counts (MSMS
   computes the external component by default), so buried cavities are
   filled.
2. A point is inside the SES when it is farther than `probe` from every
   accessible probe centre. The distance is estimated as
   max(L, E - h/2): L = max_i (r_i + p - |x - c_i|) is an exact lower bound
   (exact over contact patches), E is the Euclidean distance transform to
   the nearest accessible grid point (an upper bound, exact up to grid
   quantisation in re-entrant regions).
3. The zero level set of distance - probe is triangulated with UTisocontour
   (marching cubes); normals come from the field gradient.

Accuracy is set by the grid step h (derived from the MSMS-style vertex
density): vertices lie within ~h/4 of the analytical SES.
"""
import numpy as np

try:
    from scipy import ndimage
    from scipy.spatial import cKDTree
except ImportError as e:  # pragma: no cover
    raise ImportError("mslib needs scipy (pip install scipy)") from e

from UTpackages.UTisocontour import isocontour

isocontour.setVerboseLevel(0)

import os as _os

EDT_CORRECTION = float(_os.environ.get("MSLIB_EDT_CORR", "0.5"))
_H_OVERRIDE = _os.environ.get("MSLIB_H")


def grid_step(density):
    """Grid step (A) for an MSMS-style vertex density (vertices per A^2).

    h = 1/sqrt(density), clamped to [0.25, 0.8] A. Against MSMS on HIV
    protease (hsg1, 1641 atoms, probe 1.5): density 3 -> h 0.58 A, vertices
    within ~0.1 A (sd) of the analytical SES with no mean bias, SES area
    ~3-4% low (marching cubes cuts corners on re-entrant patches), volume
    within ~2%. See tests/sweep_msms.sh."""
    if _H_OVERRIDE:
        return float(_H_OVERRIDE)
    h = 1.0 / np.sqrt(max(float(density), 1e-3))
    return float(min(max(h, 0.25), 0.8))


def largest_component(v, n, t):
    """Keep the largest connected piece of the mesh (MSMS's external SES is
    a single connected surface; grid sampling can add small fragments)."""
    if len(t) == 0:
        return v, n, t
    from scipy.sparse import coo_matrix
    from scipy.sparse.csgraph import connected_components
    i = np.concatenate([t[:, 0], t[:, 1], t[:, 2]])
    j = np.concatenate([t[:, 1], t[:, 2], t[:, 0]])
    g = coo_matrix((np.ones(len(i), np.int8), (i, j)), shape=(len(v), len(v)))
    ncomp, lab = connected_components(g, directed=False)
    if ncomp == 1:
        return v, n, t
    keep_lab = np.bincount(lab[t[:, 0]]).argmax()
    keep_v = lab == keep_lab
    remap = -np.ones(len(v), np.int64)
    remap[keep_v] = np.arange(keep_v.sum())
    tk = t[keep_v[t[:, 0]]]
    return v[keep_v], n[keep_v], remap[tk].astype(t.dtype)


def _grid(xyz, R, h):
    pad = R.max() + 3 * h
    lo = xyz.min(axis=0) - pad
    hi = xyz.max(axis=0) + pad
    dims = np.ceil((hi - lo) / h).astype(int) + 1
    return lo.astype(np.float64), dims


def _lower_bound_field(xyz, R, origin, dims, h):
    """L(x) = max_i (R_i - |x - c_i|), clipped below at -2h (only its sign
    and values near the surface matter)."""
    L = np.full(tuple(dims), -2.0 * h, dtype=np.float32)
    ax = [origin[d] + h * np.arange(dims[d]) for d in range(3)]
    for c, r in zip(xyz, R):
        lo = np.maximum(np.floor((c - r - origin) / h).astype(int) - 1, 0)
        hi = np.minimum(np.ceil((c + r - origin) / h).astype(int) + 2, dims)
        dx = (ax[0][lo[0]:hi[0]] - c[0])[:, None, None]
        dy = (ax[1][lo[1]:hi[1]] - c[1])[None, :, None]
        dz = (ax[2][lo[2]:hi[2]] - c[2])[None, None, :]
        val = r - np.sqrt(dx * dx + dy * dy + dz * dz)
        sub = L[lo[0]:hi[0], lo[1]:hi[1], lo[2]:hi[2]]
        np.maximum(sub, val, out=sub)
    return L


def ses_field(xyz, radii, probe, h, all_components=False):
    """Return (field, origin, dims): field > 0 inside the SES."""
    xyz = np.asarray(xyz, np.float64)
    radii = np.asarray(radii, np.float64)
    R = radii + probe
    origin, dims = _grid(xyz, R, h)
    L = _lower_bound_field(xyz, R, origin, dims, h)
    inside_sas = L > 0
    exterior = ~inside_sas
    if not all_components:
        lab, n = ndimage.label(exterior)
        border = np.unique(np.concatenate([
            lab[0].ravel(), lab[-1].ravel(), lab[:, 0].ravel(), lab[:, -1].ravel(),
            lab[:, :, 0].ravel(), lab[:, :, -1].ravel()]))
        border = border[border > 0]
        exterior = np.isin(lab, border)
    # distance (A) from each point to the nearest accessible probe centre:
    # E (EDT to the nearest accessible grid point) overestimates it by up to
    # sqrt(3)/2 h; EDT_CORRECTION*h removes the mean bias (calibrated
    # against MSMS, see tests/compare_msms.sh)
    E = ndimage.distance_transform_edt(~exterior, sampling=h).astype(np.float32)
    D = np.maximum(L, E - EDT_CORRECTION * h)
    D[exterior] = 0.0
    return (D - probe).astype(np.float32), origin, dims


def triangulate(field, origin, h):
    """Marching cubes on the zero level set. Returns verts, normals, tris."""
    nx, ny, nz = field.shape
    data = np.ascontiguousarray(np.reshape(np.transpose(field), (1, 1, nx, ny, nz)), 'f')
    ds = isocontour.newDatasetRegFloat3D(data, np.asarray(origin, 'f'),
                                         np.array([h, h, h], 'f'))
    try:
        iso = isocontour.getContour3d(ds, 0, 0, 0.0, isocontour.NO_COLOR_VARIABLE)
        v = np.zeros((iso.nvert, 3), 'f')
        n = np.zeros((iso.nvert, 3), 'f')
        col = np.zeros(iso.nvert, 'f')
        t = np.zeros((iso.ntri, 3), 'i')
        if iso.nvert:
            isocontour.getContour3dData(iso, v, n, col, t, 0)
    finally:
        isocontour.delDatasetReg(ds)
    if len(t):
        # gradient-based normals point outward (field decreases outward);
        # orient each triangle to agree with them
        fn = np.cross(v[t[:, 1]] - v[t[:, 0]], v[t[:, 2]] - v[t[:, 0]])
        flip = np.einsum('ij,ij->i', fn, n[t].sum(axis=1)) < 0
        t[flip] = t[flip][:, ::-1]
    return v, n, t


def triangle_areas(v, t):
    return 0.5 * np.linalg.norm(np.cross(v[t[:, 1]] - v[t[:, 0]],
                                         v[t[:, 2]] - v[t[:, 0]]), axis=1)


def vertex_areas(v, t):
    a = triangle_areas(v, t) / 3.0
    out = np.zeros(len(v), np.float64)
    for k in range(3):
        np.add.at(out, t[:, k], a)
    return out


def enclosed_volume(v, t):
    return float(abs(np.einsum('ij,ij->i', v[t[:, 0]],
                               np.cross(v[t[:, 1]], v[t[:, 2]])).sum()) / 6.0)


def closest_atoms(v, xyz, radii, k=16):
    """0-based index of the atom whose vdW surface is closest to each vertex."""
    xyz = np.asarray(xyz, np.float64)
    if len(xyz) == 1:
        return np.zeros(len(v), int)
    k = min(k, len(xyz))
    tree = cKDTree(xyz)
    d, idx = tree.query(v, k=k)
    surf = d - np.asarray(radii)[idx]
    return idx[np.arange(len(v)), np.argmin(surf, axis=1)]


_SPHERE_CACHE = {}


def _sphere_points(n):
    if n not in _SPHERE_CACHE:
        i = np.arange(n) + 0.5
        phi = np.arccos(1 - 2 * i / n)
        theta = np.pi * (1 + 5 ** 0.5) * i
        _SPHERE_CACHE[n] = np.stack([np.cos(theta) * np.sin(phi),
                                     np.sin(theta) * np.sin(phi),
                                     np.cos(phi)], axis=1)
    return _SPHERE_CACHE[n]


def sas_atom_areas(xyz, radii, probe, npts=256):
    """Shrake-Rupley solvent accessible area per atom (A^2)."""
    xyz = np.asarray(xyz, np.float64)
    R = np.asarray(radii, np.float64) + probe
    pts = _sphere_points(npts)
    tree = cKDTree(xyz)
    rmax = R.max()
    out = np.zeros(len(xyz))
    for i, (c, r) in enumerate(zip(xyz, R)):
        nb = [j for j in tree.query_ball_point(c, r + rmax) if j != i]
        p = c + r * pts
        if nb:
            d2 = ((p[:, None, :] - xyz[nb][None, :, :]) ** 2).sum(axis=2)
            free = np.all(d2 > (R[nb] ** 2)[None, :], axis=1)
        else:
            free = np.ones(npts, bool)
        out[i] = 4 * np.pi * r * r * free.mean()
    return out


def compute(xyz, radii, probe=1.5, density=1.0, all_components=False):
    """Full pipeline; returns a dict of NumPy arrays describing the surface."""
    h = grid_step(density)
    field, origin, dims = ses_field(xyz, radii, probe, h, all_components)
    v, n, t = triangulate(field, origin, h)
    if not all_components:
        v, n, t = largest_component(v, n, t)
    atom = closest_atoms(v, xyz, radii) if len(v) else np.zeros(0, int)
    return dict(vertices=v, normals=n, triangles=t, atom=atom, h=h,
                vertex_area=vertex_areas(v, t) if len(t) else np.zeros(len(v)))
