"""numpy.oldnumeric.mlab replacement (only what MGLTools uses)."""
import numpy as _np
from numpy import *  # noqa: F401,F403


def randn(*args):
    return _np.random.standard_normal(args or None)


def rand(*args):
    return _np.random.random_sample(args or None)
