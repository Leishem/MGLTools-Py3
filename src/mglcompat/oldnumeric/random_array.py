"""numpy.oldnumeric.random_array replacement (Numeric's RandomArray)."""
import numpy as _np

_rng = _np.random


def seed(x=0, y=0):
    _rng.seed(None if (x == 0 and y == 0) else (int(x) * 2147483563 + int(y)) % (2 ** 32))


def get_seed():
    raise NotImplementedError("get_seed is not supported")


def random(shape=[]):
    return _rng.random_sample(shape or None)


def uniform(minimum, maximum, shape=[]):
    return _rng.uniform(minimum, maximum, shape or None)


def randint(minimum, maximum=None, shape=[]):
    if maximum is None:
        minimum, maximum = 0, minimum
    return _rng.randint(minimum, maximum, shape or None)


def random_integers(maximum, minimum=1, shape=[]):
    return _rng.randint(minimum, maximum + 1, shape or None)


def permutation(n):
    return _rng.permutation(n)


def standard_normal(shape=[]):
    return _rng.standard_normal(shape or None)


def normal(mean, std, shape=[]):
    return _rng.normal(mean, std, shape or None)


def multivariate_normal(mean, cov, shape=[]):
    return _rng.multivariate_normal(mean, cov, shape or None)
