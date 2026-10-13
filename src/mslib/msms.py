"""Low-level MSMS compatibility layer.

The original mslib.msms was a SWIG wrapper around the closed-source MSMS
library. This module keeps the names Pmv and other MGLTools packages use --
constants, MS_* functions and the component records reached through
``srf.rsr`` / ``srf.sesr`` -- on top of the grid-based surface in _ses.py.
"""
MS_OK = 1
MS_ERR = 0
MS_err_msg = ""

# triangulation output formats
MS_TSES_ASCII = 1
MS_ASES_ASCII = 2
MS_TSES_ASCII_AVS = 3
MS_ASES_ASCII_AVS = 4

# area computation modes
MS_NUMERICAL = 1
MS_SEMI_ANALYTICAL = 2
MS_BOTH = 3

# surface update modes
FULL = 1
TORIC = 2
ALL_WITH_DENSITY = 3
NO_TRIANGULATION = 4

# face types reported in getTriangles()[2][:, 3]
CONTACT, REENTRANT, TORIC_FACE = 1, 2, 3


class SESComponent:
    """One triangulated SES component (what srf.sesr.fst points to)."""

    def __init__(self, owner, index):
        self._owner = owner
        self.this = self
        self.index = index
        self.nxt = None
        self.density = owner.density
        self.hdensity = owner.hdensity
        self.nbtri = 0
        self.nbvert = 0
        # analytical (here: mesh-based) and numerical areas/volume
        self.a_ses_area = 0.0
        self.a_sas_area = 0.0
        self.a_reent_area = 0.0
        self.a_toric_area = 0.0
        self.a_contact_area = 0.0
        self.n_ses_area = 0.0
        self.n_sas_area = 0.0
        self.n_ses_volume = 0.0
        self.n_buried_ses_area = 0.0
        self.n_buried_sas_area = 0.0
        self.genus = 0


class RSComponent:
    """Reduced-surface component record (srf.rsr.fst); .ses is its SES."""

    def __init__(self, ses):
        self.this = self
        self.ses = ses
        self.nxt = None


class _Chain:
    """Head of a linked list of components (srf.rsr / srf.sesr)."""

    def __init__(self):
        self.fst = None
        self.lst = None
        self.nb = 0
        self.nb_free_vert = 0
        self.ffnba = 0
        self.all_comp_done = 0
        self.ext_comp_done = 0

    def set_ffat(self, *atoms):
        self.ffat = atoms

    def reset(self):
        self.__init__()

    def append(self, comp):
        if self.lst is None:
            self.fst = self.lst = comp
        else:
            self.lst.nxt = comp
            self.lst = comp
        self.nb += 1


class AtomRecord:
    """srf.get_atm(i): per-atom surface areas by component."""

    def __init__(self, ses_area, sas_area):
        self._ses = ses_area
        self._sas = sas_area

    def get_ses_area(self, component=0):
        return float(self._ses.get(component, 0.0))

    def get_sas_area(self, component=0):
        return float(self._sas.get(component, 0.0))


# --- MS_* functions operating on an MSMS object -------------------------------
def MS_reset_msms_err():
    global MS_err_msg
    MS_err_msg = ""


def MS_reset_atom_update_flag(srf):
    srf._moved = set()


def MS_updateSpheres(srf, n, indices, coords):
    """Assign new x,y,z,r to the atoms with the given (0-based) indices."""
    import numpy as np
    coords = np.asarray(coords, np.float32).reshape(-1, 4)
    idx = np.asarray(indices, int)[:n]
    srf.xyzr[idx] = coords[:n]
    srf._moved.update(int(i) for i in idx)
    return MS_OK


def MS_tagCloseProbes(srf, rs, cut):
    return MS_OK


def MS_update_surface(srf, rs, mode=FULL, density=None, update=0):
    srf.compute(probe_radius=srf.rp, density=density or srf.density,
                hdensity=srf.hdensity)
    return MS_OK


def MS_update_SES_area(srf, ses=None):
    return srf.compute_ses_area()


def MS_compute_surface(srf, probe_radius, density, hdensity):
    return srf.compute(probe_radius, density, hdensity)


def MS_info_molsrf(srf):
    return srf.info()


def MS_detailed_info_molsrf(srf):
    return srf.detailed_info()
