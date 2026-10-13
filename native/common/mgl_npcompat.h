/*
 * mgl_npcompat.h - lets the MGLTools SWIG wrappers (written for Numeric /
 * NumPy 1.x) compile against NumPy 1.22+ and 2.x.
 *
 * Included in place of "numpy/arrayobject.h" by every numarr.i / typemap file.
 * Only restores what the wrappers use; new code should use the modern API.
 */
#ifndef MGL_NPCOMPAT_H
#define MGL_NPCOMPAT_H

#include <Python.h>
#include "numpy/arrayobject.h"

/* Old type-number aliases (numpy/old_defines.h, removed in NumPy 2.0). */
#ifndef PyArray_CHAR
#define PyArray_CHAR    NPY_BYTE
#endif
#ifndef PyArray_UBYTE
#define PyArray_BYTE    NPY_BYTE
#define PyArray_UBYTE   NPY_UBYTE
#define PyArray_SHORT   NPY_SHORT
#define PyArray_USHORT  NPY_USHORT
#define PyArray_INT     NPY_INT
#define PyArray_UINT    NPY_UINT
#define PyArray_LONG    NPY_LONG
#define PyArray_ULONG   NPY_ULONG
#define PyArray_FLOAT   NPY_FLOAT
#define PyArray_DOUBLE  NPY_DOUBLE
#define PyArray_CFLOAT  NPY_CFLOAT
#define PyArray_CDOUBLE NPY_CDOUBLE
#define PyArray_OBJECT  NPY_OBJECT
#endif
#ifndef PyArray_SBYTE
#define PyArray_SBYTE   NPY_BYTE
#endif
#ifndef PyArray_NOTYPE
#define PyArray_NOTYPE  NPY_NOTYPE
#endif
#ifndef PyArray_NTYPES
#if NPY_ABI_VERSION >= 0x02000000
#define PyArray_NTYPES  NPY_NTYPES_LEGACY
#else
#define PyArray_NTYPES  NPY_NTYPES
#endif
#endif

#ifndef CONTIGUOUS
#define CONTIGUOUS NPY_ARRAY_C_CONTIGUOUS
#endif
#ifndef OWN_DATA
#define OWN_DATA NPY_ARRAY_OWNDATA
#endif
#ifndef NPY_OWNDATA
#define NPY_OWNDATA NPY_ARRAY_OWNDATA
#endif

/* PyArray_FromDims / PyArray_FromDimsAndData took int* dims (removed). */
static inline PyObject *
mgl_FromDimsAndData(int nd, const int *dims, int typenum, char *data)
{
  npy_intp d[NPY_MAXDIMS];
  int i;
  for (i = 0; i < nd && i < NPY_MAXDIMS; i++) d[i] = (npy_intp)dims[i];
  if (data == NULL)
    return PyArray_ZEROS(nd, d, typenum, 0);
  return PyArray_SimpleNewFromData(nd, d, typenum, (void *)data);
}
#undef PyArray_FromDimsAndData
#undef PyArray_FromDims
#define PyArray_FromDimsAndData(nd, dims, t, data) \
  mgl_FromDimsAndData((nd), (const int *)(dims), (t), (char *)(data))
#define PyArray_FromDims(nd, dims, t) \
  mgl_FromDimsAndData((nd), (const int *)(dims), (t), NULL)

/* The wrappers malloc() a buffer, wrap it in an array and then set
   NPY_OWNDATA so NumPy frees it. NumPy 2 frees through its own allocator
   policy, so instead hand ownership to a capsule that calls free(). */
static void mgl_capsule_free(PyObject *cap)
{
  free(PyCapsule_GetPointer(cap, "mgl_owned"));
}
static inline void mgl_set_owndata(PyObject *a)
{
  PyObject *cap;
  if (a == NULL) return;
  cap = PyCapsule_New(PyArray_DATA((PyArrayObject *)a), "mgl_owned",
                      mgl_capsule_free);
  if (cap) PyArray_SetBaseObject((PyArrayObject *)a, cap);
}
#define MGL_SET_OWNDATA(a) mgl_set_owndata((PyObject *)(a))

/* Python 2 C-API names used by hand-written wrapper code. */
#ifndef PyInt_Check
#define PyInt_Check          PyLong_Check
#define PyInt_AsLong         PyLong_AsLong
#define PyInt_AS_LONG        PyLong_AsLong
#define PyInt_FromLong       PyLong_FromLong
#endif
#ifndef PyString_Check
#define PyString_Check       PyUnicode_Check
#define PyString_FromString  PyUnicode_FromString
#define PyString_AsString    PyUnicode_AsUTF8
#define PyString_AS_STRING   PyUnicode_AsUTF8
#define PyString_Size        PyUnicode_GetLength
#define PyString_FromStringAndSize PyUnicode_FromStringAndSize
#endif

#endif /* MGL_NPCOMPAT_H */
