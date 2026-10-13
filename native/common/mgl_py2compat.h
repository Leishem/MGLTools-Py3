/*
 * mgl_py2compat.h - Python 2 C-API names used by MGLTools C code, mapped to
 * their Python 3 equivalents. Macro-only (no includes), so it can be
 * force-included in every translation unit (-include / /FI) by setup.py.
 */
#ifndef MGL_PY2COMPAT_H
#define MGL_PY2COMPAT_H

#define PyInt_Check          PyLong_Check
#define PyInt_CheckExact     PyLong_CheckExact
#define PyInt_AsLong         PyLong_AsLong
#define PyInt_AS_LONG        PyLong_AsLong
#define PyInt_FromLong       PyLong_FromLong
#define PyInt_FromSize_t     PyLong_FromSize_t
#define PyInt_Type           PyLong_Type
#define PyString_Check       PyUnicode_Check
#define PyString_FromString  PyUnicode_FromString
#define PyString_AsString    PyUnicode_AsUTF8
#define PyString_AS_STRING   PyUnicode_AsUTF8
#define PyString_Size        PyUnicode_GetLength
#define PyString_GET_SIZE    PyUnicode_GetLength
#define PyString_FromStringAndSize PyUnicode_FromStringAndSize
#define PyString_FromFormat  PyUnicode_FromFormat
#define PyNumber_Int         PyNumber_Long
#define staticforward        static
#define statichere           static

#endif /* MGL_PY2COMPAT_H */
