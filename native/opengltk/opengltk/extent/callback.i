
%{
#include "callback.h"

static PyObject_t converter_voidstar2buffer( void* ptr)
{
return PyBuffer_FromMemory( ptr, 0x100000);
}
%}

/* void ['void'] callback */

%{
/* void void_void_callback( int idx); */

static void void_void_callback( int idx)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "si", "void_void", idx);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_void");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_void_0( void)
{
  void_void_callback( 0);
}

static void WSTDCALL void_void_1( void)
{
  void_void_callback( 1);
}

static void WSTDCALL void_void_2( void)
{
  void_void_callback( 2);
}

static void WSTDCALL void_void_3( void)
{
  void_void_callback( 3);
}

static void WSTDCALL void_void_4( void)
{
  void_void_callback( 4);
}

static void WSTDCALL void_void_5( void)
{
  void_void_callback( 5);
}

static void WSTDCALL void_void_6( void)
{
  void_void_callback( 6);
}

static void WSTDCALL void_void_7( void)
{
  void_void_callback( 7);
}

static void WSTDCALL void_void_8( void)
{
  void_void_callback( 8);
}

static void WSTDCALL void_void_9( void)
{
  void_void_callback( 9);
}

static void WSTDCALL void_void_10( void)
{
  void_void_callback( 10);
}

static void WSTDCALL void_void_11( void)
{
  void_void_callback( 11);
}

static void WSTDCALL void_void_12( void)
{
  void_void_callback( 12);
}

static void WSTDCALL void_void_13( void)
{
  void_void_callback( 13);
}

static void WSTDCALL void_void_14( void)
{
  void_void_callback( 14);
}

static void_void_f void_void_array[] = {
  void_void_0,
  void_void_1,
  void_void_2,
  void_void_3,
  void_void_4,
  void_void_5,
  void_void_6,
  void_void_7,
  void_void_8,
  void_void_9,
  void_void_10,
  void_void_11,
  void_void_12,
  void_void_13,
  void_void_14,
};

#define void_void_DIM (sizeof void_void_array / sizeof *void_void_array)

static void_void_f void_void_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_void_DIM);
  return void_void_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_void_f void_void_array_get( int idx);

%constant int void_void_dim = void_void_DIM;
%constant void_void_f void_void_NULL = NULL;

/* void [('GLenum', 'which')] callback */

%{
/* void void_GLenum_callback( int idx, GLenum which); */
static void void_GLenum_callback( int idx, GLenum which)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "sii", "void_GLenum", idx, which);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_GLenum");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_GLenum_0(  GLenum which)
{
  void_GLenum_callback( 0, which);
}

static void WSTDCALL void_GLenum_1(  GLenum which)
{
  void_GLenum_callback( 1, which);
}

static void WSTDCALL void_GLenum_2(  GLenum which)
{
  void_GLenum_callback( 2, which);
}

static void WSTDCALL void_GLenum_3(  GLenum which)
{
  void_GLenum_callback( 3, which);
}

static void WSTDCALL void_GLenum_4(  GLenum which)
{
  void_GLenum_callback( 4, which);
}

static void WSTDCALL void_GLenum_5(  GLenum which)
{
  void_GLenum_callback( 5, which);
}

static void WSTDCALL void_GLenum_6(  GLenum which)
{
  void_GLenum_callback( 6, which);
}

static void WSTDCALL void_GLenum_7(  GLenum which)
{
  void_GLenum_callback( 7, which);
}

static void WSTDCALL void_GLenum_8(  GLenum which)
{
  void_GLenum_callback( 8, which);
}

static void WSTDCALL void_GLenum_9(  GLenum which)
{
  void_GLenum_callback( 9, which);
}

static void WSTDCALL void_GLenum_10(  GLenum which)
{
  void_GLenum_callback( 10, which);
}

static void WSTDCALL void_GLenum_11(  GLenum which)
{
  void_GLenum_callback( 11, which);
}

static void WSTDCALL void_GLenum_12(  GLenum which)
{
  void_GLenum_callback( 12, which);
}

static void WSTDCALL void_GLenum_13(  GLenum which)
{
  void_GLenum_callback( 13, which);
}

static void WSTDCALL void_GLenum_14(  GLenum which)
{
  void_GLenum_callback( 14, which);
}

static void_GLenum_f void_GLenum_array[] = {
  void_GLenum_0,
  void_GLenum_1,
  void_GLenum_2,
  void_GLenum_3,
  void_GLenum_4,
  void_GLenum_5,
  void_GLenum_6,
  void_GLenum_7,
  void_GLenum_8,
  void_GLenum_9,
  void_GLenum_10,
  void_GLenum_11,
  void_GLenum_12,
  void_GLenum_13,
  void_GLenum_14,
};

#define void_GLenum_DIM (sizeof void_GLenum_array / sizeof *void_GLenum_array)

static void_GLenum_f void_GLenum_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_GLenum_DIM);
  return void_GLenum_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_GLenum_f void_GLenum_array_get( int idx);

%constant int void_GLenum_dim = void_GLenum_DIM;
%constant void_GLenum_f void_GLenum_NULL = NULL;

/* void [('int', 'arg0')] callback */

%{
/* void void_int_callback( int idx, int arg0); */

static void void_int_callback( int idx, int arg0)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "sii", "void_int", idx, arg0);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_int");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_int_0(  int arg0)
{
  void_int_callback( 0, arg0);
}

static void WSTDCALL void_int_1(  int arg0)
{
  void_int_callback( 1, arg0);
}

static void WSTDCALL void_int_2(  int arg0)
{
  void_int_callback( 2, arg0);
}

static void WSTDCALL void_int_3(  int arg0)
{
  void_int_callback( 3, arg0);
}

static void WSTDCALL void_int_4(  int arg0)
{
  void_int_callback( 4, arg0);
}

static void WSTDCALL void_int_5(  int arg0)
{
  void_int_callback( 5, arg0);
}

static void WSTDCALL void_int_6(  int arg0)
{
  void_int_callback( 6, arg0);
}

static void WSTDCALL void_int_7(  int arg0)
{
  void_int_callback( 7, arg0);
}

static void WSTDCALL void_int_8(  int arg0)
{
  void_int_callback( 8, arg0);
}

static void WSTDCALL void_int_9(  int arg0)
{
  void_int_callback( 9, arg0);
}

static void WSTDCALL void_int_10(  int arg0)
{
  void_int_callback( 10, arg0);
}

static void WSTDCALL void_int_11(  int arg0)
{
  void_int_callback( 11, arg0);
}

static void WSTDCALL void_int_12(  int arg0)
{
  void_int_callback( 12, arg0);
}

static void WSTDCALL void_int_13(  int arg0)
{
  void_int_callback( 13, arg0);
}

static void WSTDCALL void_int_14(  int arg0)
{
  void_int_callback( 14, arg0);
}

static void_int_f void_int_array[] = {
  void_int_0,
  void_int_1,
  void_int_2,
  void_int_3,
  void_int_4,
  void_int_5,
  void_int_6,
  void_int_7,
  void_int_8,
  void_int_9,
  void_int_10,
  void_int_11,
  void_int_12,
  void_int_13,
  void_int_14,
};

#define void_int_DIM (sizeof void_int_array / sizeof *void_int_array)

static void_int_f void_int_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_int_DIM);
  return void_int_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_int_f void_int_array_get( int idx);

%constant int void_int_dim = void_int_DIM;
%constant void_int_f void_int_NULL = NULL;

/* void [('int', 'arg0'), ('int', 'arg1')] callback */

%{
/* void void_int_int_callback( int idx, int arg0, int arg1); */

static void void_int_int_callback( int idx, int arg0, int arg1)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "siii", "void_int_int", idx, arg0, arg1);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_int_int");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_int_int_0(  int arg0, int arg1)
{
  void_int_int_callback( 0, arg0, arg1);
}

static void WSTDCALL void_int_int_1(  int arg0, int arg1)
{
  void_int_int_callback( 1, arg0, arg1);
}

static void WSTDCALL void_int_int_2(  int arg0, int arg1)
{
  void_int_int_callback( 2, arg0, arg1);
}

static void WSTDCALL void_int_int_3(  int arg0, int arg1)
{
  void_int_int_callback( 3, arg0, arg1);
}

static void WSTDCALL void_int_int_4(  int arg0, int arg1)
{
  void_int_int_callback( 4, arg0, arg1);
}

static void WSTDCALL void_int_int_5(  int arg0, int arg1)
{
  void_int_int_callback( 5, arg0, arg1);
}

static void WSTDCALL void_int_int_6(  int arg0, int arg1)
{
  void_int_int_callback( 6, arg0, arg1);
}

static void WSTDCALL void_int_int_7(  int arg0, int arg1)
{
  void_int_int_callback( 7, arg0, arg1);
}

static void WSTDCALL void_int_int_8(  int arg0, int arg1)
{
  void_int_int_callback( 8, arg0, arg1);
}

static void WSTDCALL void_int_int_9(  int arg0, int arg1)
{
  void_int_int_callback( 9, arg0, arg1);
}

static void WSTDCALL void_int_int_10(  int arg0, int arg1)
{
  void_int_int_callback( 10, arg0, arg1);
}

static void WSTDCALL void_int_int_11(  int arg0, int arg1)
{
  void_int_int_callback( 11, arg0, arg1);
}

static void WSTDCALL void_int_int_12(  int arg0, int arg1)
{
  void_int_int_callback( 12, arg0, arg1);
}

static void WSTDCALL void_int_int_13(  int arg0, int arg1)
{
  void_int_int_callback( 13, arg0, arg1);
}

static void WSTDCALL void_int_int_14(  int arg0, int arg1)
{
  void_int_int_callback( 14, arg0, arg1);
}

static void_int_int_f void_int_int_array[] = {
  void_int_int_0,
  void_int_int_1,
  void_int_int_2,
  void_int_int_3,
  void_int_int_4,
  void_int_int_5,
  void_int_int_6,
  void_int_int_7,
  void_int_int_8,
  void_int_int_9,
  void_int_int_10,
  void_int_int_11,
  void_int_int_12,
  void_int_int_13,
  void_int_int_14,
};

#define void_int_int_DIM (sizeof void_int_int_array / sizeof *void_int_int_array)

static void_int_int_f void_int_int_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_int_int_DIM);
  return void_int_int_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_int_int_f void_int_int_array_get( int idx);

%constant int void_int_int_dim = void_int_int_DIM;
%constant void_int_int_f void_int_int_NULL = NULL;

/* void [('int', 'arg0'), ('int', 'arg1'), ('int', 'arg2')] callback */

%{
  /*void void_int_int_int_callback( int idx, int arg0, int arg1, int arg2); */

static void void_int_int_int_callback( int idx, int arg0, int arg1, int arg2)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "siiii", "void_int_int_int", idx, arg0, arg1, arg2);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_int_int_int");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_int_int_int_0(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 0, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_1(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 1, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_2(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 2, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_3(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 3, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_4(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 4, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_5(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 5, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_6(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 6, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_7(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 7, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_8(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 8, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_9(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 9, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_10(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 10, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_11(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 11, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_12(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 12, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_13(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 13, arg0, arg1, arg2);
}

static void WSTDCALL void_int_int_int_14(  int arg0, int arg1, int arg2)
{
  void_int_int_int_callback( 14, arg0, arg1, arg2);
}

static void_int_int_int_f void_int_int_int_array[] = {
  void_int_int_int_0,
  void_int_int_int_1,
  void_int_int_int_2,
  void_int_int_int_3,
  void_int_int_int_4,
  void_int_int_int_5,
  void_int_int_int_6,
  void_int_int_int_7,
  void_int_int_int_8,
  void_int_int_int_9,
  void_int_int_int_10,
  void_int_int_int_11,
  void_int_int_int_12,
  void_int_int_int_13,
  void_int_int_int_14,
};

#define void_int_int_int_DIM (sizeof void_int_int_int_array / sizeof *void_int_int_int_array)

static void_int_int_int_f void_int_int_int_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_int_int_int_DIM);
  return void_int_int_int_array[ idx];
}

%}

%include gltypedef.i
%include callback.h
void_int_int_int_f void_int_int_int_array_get( int idx);

%constant int void_int_int_int_dim = void_int_int_int_DIM;
%constant void_int_int_int_f void_int_int_int_NULL = NULL;

/* void [('int', 'arg0'), ('int', 'arg1'), ('int', 'arg2'), ('int', 'arg3')] callback */

%{
  /* void void_int_int_int_int_callback( int idx, int arg0, int arg1, int arg2, int arg3); */
static void void_int_int_int_int_callback( int idx, int arg0, int arg1, int arg2, int arg3)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "siiiii", "void_int_int_int_int", idx, arg0, arg1, arg2, arg3);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_int_int_int_int");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_int_int_int_int_0(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 0, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_1(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 1, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_2(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 2, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_3(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 3, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_4(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 4, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_5(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 5, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_6(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 6, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_7(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 7, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_8(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 8, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_9(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 9, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_10(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 10, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_11(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 11, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_12(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 12, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_13(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 13, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_int_int_int_int_14(  int arg0, int arg1, int arg2, int arg3)
{
  void_int_int_int_int_callback( 14, arg0, arg1, arg2, arg3);
}

static void_int_int_int_int_f void_int_int_int_int_array[] = {
  void_int_int_int_int_0,
  void_int_int_int_int_1,
  void_int_int_int_int_2,
  void_int_int_int_int_3,
  void_int_int_int_int_4,
  void_int_int_int_int_5,
  void_int_int_int_int_6,
  void_int_int_int_int_7,
  void_int_int_int_int_8,
  void_int_int_int_int_9,
  void_int_int_int_int_10,
  void_int_int_int_int_11,
  void_int_int_int_int_12,
  void_int_int_int_int_13,
  void_int_int_int_int_14,
};

#define void_int_int_int_int_DIM (sizeof void_int_int_int_int_array / sizeof *void_int_int_int_int_array)

static void_int_int_int_int_f void_int_int_int_int_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_int_int_int_int_DIM);
  return void_int_int_int_int_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_int_int_int_int_f void_int_int_int_int_array_get( int idx);

%constant int void_int_int_int_int_dim = void_int_int_int_int_DIM;
%constant void_int_int_int_int_f void_int_int_int_int_NULL = NULL;

/* void [('unsigned char', 'arg0'), ('int', 'arg1'), ('int', 'arg2')] callback */

%{
  /* void void_unsignedchar_int_int_callback( int idx, unsigned char arg0, int arg1, int arg2); */

static void void_unsignedchar_int_int_callback( int idx, unsigned char arg0, int arg1, int arg2)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "siiii", "void_unsignedchar_int_int", idx, arg0, arg1, arg2);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_unsignedchar_int_int");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_unsignedchar_int_int_0(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 0, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_1(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 1, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_2(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 2, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_3(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 3, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_4(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 4, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_5(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 5, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_6(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 6, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_7(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 7, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_8(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 8, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_9(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 9, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_10(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 10, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_11(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 11, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_12(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 12, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_13(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 13, arg0, arg1, arg2);
}

static void WSTDCALL void_unsignedchar_int_int_14(  unsigned char arg0, int arg1, int arg2)
{
  void_unsignedchar_int_int_callback( 14, arg0, arg1, arg2);
}

static void_unsignedchar_int_int_f void_unsignedchar_int_int_array[] = {
  void_unsignedchar_int_int_0,
  void_unsignedchar_int_int_1,
  void_unsignedchar_int_int_2,
  void_unsignedchar_int_int_3,
  void_unsignedchar_int_int_4,
  void_unsignedchar_int_int_5,
  void_unsignedchar_int_int_6,
  void_unsignedchar_int_int_7,
  void_unsignedchar_int_int_8,
  void_unsignedchar_int_int_9,
  void_unsignedchar_int_int_10,
  void_unsignedchar_int_int_11,
  void_unsignedchar_int_int_12,
  void_unsignedchar_int_int_13,
  void_unsignedchar_int_int_14,
};

#define void_unsignedchar_int_int_DIM (sizeof void_unsignedchar_int_int_array / sizeof *void_unsignedchar_int_int_array)

static void_unsignedchar_int_int_f void_unsignedchar_int_int_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_unsignedchar_int_int_DIM);
  return void_unsignedchar_int_int_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_unsignedchar_int_int_f void_unsignedchar_int_int_array_get( int idx);

%constant int void_unsignedchar_int_int_dim = void_unsignedchar_int_int_DIM;
%constant void_unsignedchar_int_int_f void_unsignedchar_int_int_NULL = NULL;

/* void [('unsigned int', 'arg0'), ('int', 'arg1'), ('int', 'arg2'), ('int', 'arg3')] callback */

%{
  /* void void_unsignedint_int_int_int_callback( int idx, unsigned int arg0, int arg1, int arg2, int arg3); */

static void void_unsignedint_int_int_int_callback( int idx, unsigned int arg0, int arg1, int arg2, int arg3)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "siiiii", "void_unsignedint_int_int_int", idx, arg0, arg1, arg2, arg3);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_unsignedint_int_int_int");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}

static void WSTDCALL void_unsignedint_int_int_int_0(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 0, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_1(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 1, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_2(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 2, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_3(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 3, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_4(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 4, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_5(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 5, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_6(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 6, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_7(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 7, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_8(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 8, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_9(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 9, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_10(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 10, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_11(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 11, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_12(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 12, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_13(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 13, arg0, arg1, arg2, arg3);
}

static void WSTDCALL void_unsignedint_int_int_int_14(  unsigned int arg0, int arg1, int arg2, int arg3)
{
  void_unsignedint_int_int_int_callback( 14, arg0, arg1, arg2, arg3);
}

static void_unsignedint_int_int_int_f void_unsignedint_int_int_int_array[] = {
  void_unsignedint_int_int_int_0,
  void_unsignedint_int_int_int_1,
  void_unsignedint_int_int_int_2,
  void_unsignedint_int_int_int_3,
  void_unsignedint_int_int_int_4,
  void_unsignedint_int_int_int_5,
  void_unsignedint_int_int_int_6,
  void_unsignedint_int_int_int_7,
  void_unsignedint_int_int_int_8,
  void_unsignedint_int_int_int_9,
  void_unsignedint_int_int_int_10,
  void_unsignedint_int_int_int_11,
  void_unsignedint_int_int_int_12,
  void_unsignedint_int_int_int_13,
  void_unsignedint_int_int_int_14,
};

#define void_unsignedint_int_int_int_DIM (sizeof void_unsignedint_int_int_int_array / sizeof *void_unsignedint_int_int_int_array)

static void_unsignedint_int_int_int_f void_unsignedint_int_int_int_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_unsignedint_int_int_int_DIM);
  return void_unsignedint_int_int_int_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_unsignedint_int_int_int_f void_unsignedint_int_int_int_array_get( int idx);

%constant int void_unsignedint_int_int_int_dim = void_unsignedint_int_int_int_DIM;
%constant void_unsignedint_int_int_int_f void_unsignedint_int_int_int_NULL = NULL;

/* void [('int', 'view'), ('void*', 'context')] callback */

%{
  /* void void_int_voidstar_callback( int idx, int view, void* context); */

static void void_int_voidstar_callback( int idx, int view, void* context)
{
  static PyObject_t s_runcallback = NULL;
  PyObject_t result;

  if (utilexport.pythread) PyEval_AcquireThread( utilexport.pythread);
  if (NOT s_runcallback)
    s_runcallback = PypImport_ModuleAttr( "opengltk.ccallback", "swigcallback");
  if (NOT s_runcallback) goto FUNEXIT;
  result = PyObject_CallFunction( s_runcallback, "siiO&", "void_int_voidstar", idx, view, converter_voidstar2buffer, context);
  if (NOT result) goto FUNEXIT;
  Py_DECREF( result);
FUNEXIT:
  if (PyErr_Occurred()) PypCallback_ProcessErr( "void_int_voidstar");
  if (utilexport.pythread) PyEval_ReleaseThread( utilexport.pythread);
  return;
}


static void WSTDCALL void_int_voidstar_0(  int view, void* context)
{
  void_int_voidstar_callback( 0, view, context);
}

static void WSTDCALL void_int_voidstar_1(  int view, void* context)
{
  void_int_voidstar_callback( 1, view, context);
}

static void WSTDCALL void_int_voidstar_2(  int view, void* context)
{
  void_int_voidstar_callback( 2, view, context);
}

static void WSTDCALL void_int_voidstar_3(  int view, void* context)
{
  void_int_voidstar_callback( 3, view, context);
}

static void WSTDCALL void_int_voidstar_4(  int view, void* context)
{
  void_int_voidstar_callback( 4, view, context);
}

static void WSTDCALL void_int_voidstar_5(  int view, void* context)
{
  void_int_voidstar_callback( 5, view, context);
}

static void WSTDCALL void_int_voidstar_6(  int view, void* context)
{
  void_int_voidstar_callback( 6, view, context);
}

static void WSTDCALL void_int_voidstar_7(  int view, void* context)
{
  void_int_voidstar_callback( 7, view, context);
}

static void WSTDCALL void_int_voidstar_8(  int view, void* context)
{
  void_int_voidstar_callback( 8, view, context);
}

static void WSTDCALL void_int_voidstar_9(  int view, void* context)
{
  void_int_voidstar_callback( 9, view, context);
}

static void WSTDCALL void_int_voidstar_10(  int view, void* context)
{
  void_int_voidstar_callback( 10, view, context);
}

static void WSTDCALL void_int_voidstar_11(  int view, void* context)
{
  void_int_voidstar_callback( 11, view, context);
}

static void WSTDCALL void_int_voidstar_12(  int view, void* context)
{
  void_int_voidstar_callback( 12, view, context);
}

static void WSTDCALL void_int_voidstar_13(  int view, void* context)
{
  void_int_voidstar_callback( 13, view, context);
}

static void WSTDCALL void_int_voidstar_14(  int view, void* context)
{
  void_int_voidstar_callback( 14, view, context);
}

static void_int_voidstar_f void_int_voidstar_array[] = {
  void_int_voidstar_0,
  void_int_voidstar_1,
  void_int_voidstar_2,
  void_int_voidstar_3,
  void_int_voidstar_4,
  void_int_voidstar_5,
  void_int_voidstar_6,
  void_int_voidstar_7,
  void_int_voidstar_8,
  void_int_voidstar_9,
  void_int_voidstar_10,
  void_int_voidstar_11,
  void_int_voidstar_12,
  void_int_voidstar_13,
  void_int_voidstar_14,
};

#define void_int_voidstar_DIM (sizeof void_int_voidstar_array / sizeof *void_int_voidstar_array)

static void_int_voidstar_f void_int_voidstar_array_get( int idx)
{
  assert( 0 <= idx);
  assert( idx < void_int_voidstar_DIM);
  return void_int_voidstar_array[ idx];
}


%}

%include gltypedef.i
%include callback.h
void_int_voidstar_f void_int_voidstar_array_get( int idx);

%constant int void_int_voidstar_dim = void_int_voidstar_DIM;
%constant void_int_voidstar_f void_int_voidstar_NULL = NULL;

