"""numpy.oldnumeric.linear_algebra replacement (Numeric's LinearAlgebra)."""
import numpy as _np
import numpy.linalg as _la

LinAlgError = _la.LinAlgError


def inverse(a):
    return _la.inv(a)


inv = inverse


def solve_linear_equations(a, b):
    return _la.solve(a, b)


def determinant(a):
    return _la.det(a)


def eigenvalues(a):
    return _la.eigvals(a)


def eigenvectors(a):
    w, v = _la.eig(a)
    return w, _np.transpose(v)


def Heigenvalues(a, UPLO='L'):
    return _la.eigvalsh(a, UPLO)


def Heigenvectors(a):
    w, v = _la.eigh(a)
    return w, _np.transpose(v)


def singular_value_decomposition(a, full_matrices=0):
    return _la.svd(a, bool(full_matrices))


def generalized_inverse(a, rcond=1e-10):
    return _la.pinv(a, rcond)


def linear_least_squares(a, b, rcond=1e-10):
    x, resids, rank, s = _la.lstsq(a, b, rcond=rcond)
    return x, resids, rank, s


def cholesky_decomposition(a):
    return _la.cholesky(a)
