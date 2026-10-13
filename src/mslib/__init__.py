"""mslib -- molecular surfaces for MGLTools, without the MSMS binaries.

Drop-in replacement for the MSMS-based mslib of MGLTools 1.5.6. The MSMS
class keeps the original Python API (compute, getTriangles, areas, buried
surface, updates), so Pmv/ADT surface commands work unchanged; the surface
itself is computed by mslib._ses (grid SES + marching cubes, numpy/scipy and
the UTisocontour extension), which builds on every platform MGLTools runs on.

getTriangles() returns, as MSMS did:
  vfloat (n, 8): x, y, z, nx, ny, nz, sesArea, sasArea per vertex
  vint   (n, 3): type, closest atom (1-based), buried flag per vertex
  tri    (m, 5): i, j, k, face type, face number per triangle
"""
import os

import numpy as np

from . import msms
from . import _ses

_msms = msms  # older code imported both names

__all__ = ["MSMS", "msms", "readxyzr"]


def readxyzr(filename):
    """xyzr, names, surfflags, hdflags <- readxyzr(filename)"""
    xyzr, names = [], []
    with open(filename) as f:
        for line in f:
            w = line.split()
            if len(w) < 4:
                continue
            xyzr.append([float(x) for x in w[:4]])
            names.append(w[5] if len(w) > 5 else (w[4] if len(w) > 4 else ""))
    return np.array(xyzr, np.float32), names, None, None


class MSMS:
    """High level molecule surface class (MSMS-compatible API)."""

    def __init__(self, coords=None, radii=None, atnames=None, filename=None,
                 name='msms', maxnat=0, surfflags=None, hdflags=None):
        if filename:
            c, atnames, surfflags, hdflags = readxyzr(filename)
            self.surface_name = name or os.path.basename(filename)
        else:
            self.surface_name = name
            if coords is not None and len(coords):
                c = np.array(coords, np.float32)
                assert c.ndim == 2, "coordinates array of bad shape"
                if c.shape[1] == 3:
                    r = np.array(radii, np.float32).reshape(-1, 1)
                    c = np.concatenate((c, r), 1)
                else:
                    assert c.shape[1] == 4, "coordinates array of bad shape"
            else:
                c = np.zeros((0, 4), np.float32)
        if surfflags is not None:
            assert len(surfflags) == len(c)
        if hdflags is not None:
            assert len(hdflags) == len(c)
        self.xyzr = np.ascontiguousarray(c, np.float32)
        self.coords = self.xyzr[:, :3]
        self.radii = self.xyzr[:, 3:4]
        self.names = list(atnames) if atnames is not None else None
        self.surfflags = surfflags
        self.hdflags = hdflags
        self.nbat = len(self.xyzr)
        self.maxat = max(maxnat, self.nbat)
        self.rp = 1.5
        self.rp1 = 1.0 / self.rp
        self.density = 1.0
        self.hdensity = 3.0
        self.all_components = 0
        self.try_num = 0
        self.cleanup_ses = 0
        self.geometry = None
        self.this = self
        self.rsr = msms._Chain()
        self.sesr = msms._Chain()
        self._surfaces = []          # per component: dict from _ses.compute
        self._atom_ses = {}          # component -> {atom: area}
        self._atom_sas = {}
        self._moved = set()

    # --- components -------------------------------------------------------
    def _getSESComp(self, num):
        comp = self.sesr.fst
        for i in range(num):
            comp = comp.nxt
        return comp

    def _getRSComp(self, num):
        comp = self.rsr.fst
        for i in range(num):
            comp = comp.nxt
        return comp

    # --- computation ------------------------------------------------------
    def compute(self, probe_radius=1.5, density=1.0, hdensity=3.0):
        """err <- compute(probe_radius=1.5, density=1.0, hdensity=3.0)

    Compute and triangulate the external solvent excluded surface.
    Return MS_OK on success, MS_ERR (with msms.MS_err_msg) on failure."""
        msms.MS_reset_msms_err()
        if probe_radius < 0.1:
            msms.MS_err_msg = "Probe radius has to be > 0.1"
            return msms.MS_ERR
        self.rp = float(probe_radius)
        self.rp1 = 1.0 / self.rp
        self.density = float(density)
        self.hdensity = float(hdensity)
        self.try_num += 1
        self.rsr.reset()
        self.sesr.reset()
        self._surfaces = []
        if self.nbat == 0:
            msms.MS_err_msg = "no atoms"
            return msms.MS_ERR
        try:
            surf = _ses.compute(self.xyzr[:, :3], self.xyzr[:, 3], self.rp,
                                self.density, bool(self.all_components))
        except Exception as e:  # noqa: BLE001
            msms.MS_err_msg = str(e)
            return msms.MS_ERR
        self._add_component(surf)
        return msms.MS_OK

    def _add_component(self, surf):
        idx = len(self._surfaces)
        self._surfaces.append(surf)
        v, t = surf["vertices"], surf["triangles"]
        surf["buried"] = np.zeros(len(v), np.int32)
        ses = msms.SESComponent(self, idx)
        ses.nbtri, ses.nbvert = len(t), len(v)
        area = float(surf["vertex_area"].sum())
        ses.a_ses_area = ses.n_ses_area = area
        ses.n_ses_volume = _ses.enclosed_volume(v, t) if len(t) else 0.0
        self.sesr.append(ses)
        self.rsr.append(msms.RSComponent(ses))

    def compute_rs(self, probe_radius=1.5, allComponents=0, atoms=None):
        self.all_components = allComponents
        return self.compute(probe_radius, self.density, self.hdensity)

    def compute_ses(self, component=None):
        return msms.MS_OK

    def triangulate(self, density=1.0, component=None):
        if abs(density - self.density) > 1e-6 or not self._surfaces:
            return self.compute(self.rp, density, self.hdensity)
        return msms.MS_OK

    def triangulate_one_component(self, rs, density=1.0, hdensity=3.0):
        return self.triangulate(density)

    # --- triangle access ---------------------------------------------------
    def getTriangles(self, atomIndices=None, selnum=1, base=0, component=0,
                     keepOriginalIndices=0):
        """vfloat, vint, tri <- getTriangles(atomIndices=None, selnum=1, base=0,
                                             component=0, keepOriginalIndices=0)

    atomIndices: 0-based indices of atoms whose surface is wanted (None: all).
    A triangle is returned when at least `selnum` of its vertices are closest
    to one of these atoms. With keepOriginalIndices all vertices are returned
    and triangles index into them; otherwise unused vertices are dropped."""
        s = self._surfaces[component]
        v, n, t, atom = s["vertices"], s["normals"], s["triangles"], s["atom"]
        nv = len(v)
        vfloat = np.zeros((nv, 8), np.float32)
        vfloat[:, :3] = v
        vfloat[:, 3:6] = n
        vfloat[:, 6] = s["vertex_area"]
        vfloat[:, 7] = s.get("vertex_sas_area", 0.0)
        vint = np.zeros((nv, 3), np.int32)
        vint[:, 0] = -1
        vint[:, 1] = atom + 1
        vint[:, 2] = s["buried"]
        tri = np.zeros((len(t), 5), np.int32)
        tri[:, :3] = t
        tri[:, 3] = msms.CONTACT
        tri[:, 4] = atom[t[:, 0]] + 1 if len(t) else 0
        if atomIndices is not None:
            sel = np.zeros(self.nbat, bool)
            ai = np.asarray(atomIndices, int)
            sel[ai[(ai >= 0) & (ai < self.nbat)]] = True
            onv = sel[atom]
            keep = onv[t].sum(axis=1) >= selnum if len(t) else np.zeros(0, bool)
            tri = tri[keep]
        if not keepOriginalIndices and atomIndices is not None:
            used = np.unique(tri[:, :3])
            remap = -np.ones(nv, np.int32)
            remap[used] = np.arange(len(used), dtype=np.int32)
            vfloat, vint = vfloat[used], vint[used]
            tri[:, :3] = remap[tri[:, :3]]
        if base:
            tri[:, :3] += base
        return vfloat, vint, tri

    # --- areas and volume -----------------------------------------------------
    def compute_ses_area(self):
        """Per-atom SES (from the triangulation) and SAS (Shrake-Rupley)
    areas, plus component totals in sesr.fst.a_ses_area / a_sas_area."""
        if not self._surfaces:
            return msms.MS_ERR
        sas = _ses.sas_atom_areas(self.xyzr[:, :3], self.xyzr[:, 3], self.rp)
        comp = self.sesr.fst
        for i, s in enumerate(self._surfaces):
            ses_atom = np.zeros(self.nbat)
            np.add.at(ses_atom, s["atom"], s["vertex_area"])
            self._atom_ses[i] = dict(enumerate(ses_atom))
            # SAS belongs to the external component
            self._atom_sas[i] = dict(enumerate(sas)) if i == 0 else {}
            # distribute each atom's SAS over its surface vertices
            cnt = np.bincount(s["atom"], minlength=self.nbat).astype(float)
            per_v = np.where(cnt > 0, (sas if i == 0 else 0) / np.maximum(cnt, 1), 0.0)
            s["vertex_sas_area"] = per_v[s["atom"]] if i == 0 else 0.0
            comp.a_ses_area = float(ses_atom.sum())
            comp.a_contact_area = comp.a_ses_area
            comp.a_sas_area = float(sas.sum()) if i == 0 else 0.0
            comp.n_sas_area = comp.a_sas_area
            comp = comp.nxt
        return msms.MS_OK

    def compute_numeric_area_vol(self, component=None, mode=msms.MS_SEMI_ANALYTICAL):
        for i, s in enumerate(self._surfaces):
            if component is not None and i != component:
                continue
            comp = self._getSESComp(i)
            comp.n_ses_area = float(s["vertex_area"].sum())
            comp.n_ses_volume = (_ses.enclosed_volume(s["vertices"], s["triangles"])
                                 if len(s["triangles"]) else 0.0)
        return msms.MS_OK

    def get_atm(self, i):
        return msms.AtomRecord({c: d.get(i, 0.0) for c, d in self._atom_ses.items()},
                               {c: d.get(i, 0.0) for c, d in self._atom_sas.items()})

    def write_ses_area(self, filename, component=None):
        if not self._atom_ses:
            self.compute_ses_area()
        comps = range(len(self._surfaces)) if component is None else [component]
        for i in comps:
            fn = filename if component is not None else "%s_%d" % (filename, i)
            with open(fn, "w") as f:
                f.write("# Atom  Ses_area  Sas_area\n")
                for a in range(self.nbat):
                    f.write("%6d %10.3f %10.3f\n" % (a, self._atom_ses[i].get(a, 0.0),
                                                     self._atom_sas[i].get(a, 0.0)))

    # --- buried surface -----------------------------------------------------
    def resetBuriedVertexFlags(self, component=None):
        for i, s in enumerate(self._surfaces):
            if component is None or i == component:
                s["buried"][:] = 0

    def resetBuriedVertexArea(self, component=None):
        for i, comp in enumerate(self._iter_ses()):
            if component is None or i == component:
                comp.n_buried_ses_area = comp.n_buried_sas_area = 0.0

    def _iter_ses(self):
        c = self.sesr.fst
        while c:
            yield c
            c = c.nxt

    def buriedVertices(self, coords, radii=None, component=None):
        """Tag the vertices of the SES buried by a set of spheres
    (coords Nx4, or Nx3 with radii) -- buried means inside a probe-inflated
    sphere, as in MSMS."""
        from scipy.spatial import cKDTree
        c = np.asarray(coords, np.float64)
        if radii is not None:
            r = np.asarray(radii, np.float64).ravel()
        else:
            c, r = c[:, :3], c[:, 3]
        reach = r + self.rp
        tree = cKDTree(c)
        rmax = reach.max() if len(reach) else 0.0
        for i, s in enumerate(self._surfaces):
            if component is not None and i != component:
                continue
            s["buried"][:] = 0
            v = s["vertices"]
            for vi, nb in enumerate(tree.query_ball_point(v, rmax)):
                if nb:
                    d = np.linalg.norm(c[nb] - v[vi], axis=1)
                    if np.any(d <= reach[nb]):
                        s["buried"][vi] = 1

    def buriedSurfaceArea(self, component=None, mode=msms.MS_SEMI_ANALYTICAL):
        areas = {'ses': [], 'sas': []}
        for i, (s, comp) in enumerate(zip(self._surfaces, self._iter_ses())):
            if component is not None and i != component:
                continue
            b = s["buried"].astype(bool)
            comp.n_buried_ses_area = float(s["vertex_area"][b].sum())
            vs = s.get("vertex_sas_area", 0.0)
            comp.n_buried_sas_area = float(np.asarray(vs)[b].sum()) if np.ndim(vs) else 0.0
            areas['ses'].append(comp.n_buried_ses_area)
            areas['sas'].append(comp.n_buried_sas_area)
        return areas

    def getBuriedSurfaceTriangles(self, atomIndices=None, component=0, selnum=1, negate=0):
        assert selnum in (1, 2, 3)
        vfloat, vint, tri = self.getTriangles(atomIndices, component=component,
                                              keepOriginalIndices=1)
        flag = vint[:, 2].astype(bool)
        if negate:
            flag = ~flag
        keep = flag[tri[:, :3]].sum(axis=1) >= selnum
        return vfloat, vint, tri[keep]

    # --- updates (recompute) ---------------------------------------------------
    def initUpdateSurface(self, cut=15.0, cleanup_ses=0):
        self.cleanup_ses = cleanup_ses
        msms.MS_reset_atom_update_flag(self)

    def updateSpheres(self, coords, indices):
        msms.MS_reset_atom_update_flag(self)
        return msms.MS_updateSpheres(self, len(coords), indices, coords)

    def updateCoordsFromFile(self, filename, max=-1):
        xyzr, _, _, _ = readxyzr(filename)
        n = len(xyzr) if max < 0 else min(max, len(xyzr))
        self.xyzr[:n] = xyzr[:n]

    def updateSurface(self, mode=msms.FULL, density=1.0, update=0):
        return msms.MS_update_surface(self, self.rsr.fst, mode, density, update)

    def updateSurfaceArea(self):
        return self.compute_ses_area()

    def update(self, mode=msms.FULL, density=None, nup=0):
        return self.compute(self.rp, density or self.density, self.hdensity)

    # --- output ------------------------------------------------------------------
    def write_triangulation(self, filename, no_header=1, component=None,
                            format=msms.MS_TSES_ASCII):
        """Write MSMS-format .vert/.face files."""
        comps = range(len(self._surfaces)) if component is None else [component]
        for i in comps:
            base = filename if component is not None else "%s_%d" % (filename, i)
            vf, vi, tri = self.getTriangles(component=i)
            with open(base + ".vert", "w") as f:
                if not no_header:
                    f.write("# MSMS solvent excluded surface vertices for %s\n" % self.surface_name)
                    f.write("#vertex #sphere density probe_r\n")
                    f.write("%7d %7d %6.2f %6.2f\n" % (len(vf), self.nbat, self.density, self.rp))
                for (x, y, z, nx, ny, nz, _, _), (typ, at, _) in zip(vf, vi):
                    f.write("%9.3f %9.3f %9.3f %9.3f %9.3f %9.3f %7d %7d %2d\n"
                            % (x, y, z, nx, ny, nz, 0, at, 1))
            with open(base + ".face", "w") as f:
                if not no_header:
                    f.write("# MSMS solvent excluded surface faces for %s\n" % self.surface_name)
                    f.write("#faces  #sphere density probe_r\n")
                    f.write("%7d %7d %6.2f %6.2f\n" % (len(tri), self.nbat, self.density, self.rp))
                for i0, i1, i2, typ, fn in tri:
                    f.write("%6d %6d %6d %2d %6d\n" % (i0 + 1, i1 + 1, i2 + 1, typ, fn))

    def info(self):
        lines = ["MSMS-compatible surface %s: %d atoms, probe %.2f, density %.2f"
                 % (self.surface_name, self.nbat, self.rp, self.density)]
        for i, c in enumerate(self._iter_ses()):
            lines.append("  component %d: %d vertices, %d triangles, area %.2f, volume %.2f"
                         % (i, c.nbvert, c.nbtri, c.n_ses_area, c.n_ses_volume))
        return "\n".join(lines)

    detailed_info = info

    def getColorByType(self, component=None):
        colors = np.array(((1., 0., 1.), (1., 0.5, 0.), (0., 1., 0.),
                           (1., 0., 0.), (0., 0., 1.)))
        if component is None:
            return [colors[self.getTriangles(component=i)[2][:, 3] - 1]
                    for i in range(len(self._surfaces))]
        return colors[self.getTriangles(component=component)[2][:, 3] - 1]
