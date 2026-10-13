/*
 * Copyright_notice
 */
%module prmlib

%init %{
  PyEval_InitThreads();
  mme_initCallbacks();
  import_array(); /* load the Numeric PyCObjects */
%}

%{

#ifdef _MSC_VER
#include <windows.h>
#define WinVerMajor() LOBYTE(LOWORD(GetVersion()))
#endif

#include <stdlib.h>
#include "numpy/oldnumeric.h"
#include "prm.h"

static parmstruct* parmcalloc( void)
{
  return (parmstruct*)calloc( 1, sizeof (parmstruct));
}

static void parmfree( parmstruct* parm)
{
  free( parm);
}

static PyThreadState* s_pythread = NULL;

void foo (int *obj)
{
  printf ("foo: got int *\n");
}

%}
void foo (int *obj);
//void foo1(void *obj);

%include typemaps.i

// Note: these typemaps require that the memory be held in Python

%typemap(in)
  _REAL*,
  int*
{
  long buffer_len;
  if (PyObject_AsWriteBuffer( $input, (void**)&$1, &buffer_len))
    return NULL;
}

/***********************************
%typemap(in)
  char*
{
  int buffer_len;
  if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
    return NULL;
}

************************************/

/* must be called once and only once (see wrapper) */
/******
%typemap(out)
  _REAL*,
  int*,
  char*
{
  if ($1) $result = PyCObject_FromVoidPtr( $1, free);
  else
    {
      Py_INCREF( Py_None);
      $result = Py_None;
    }
}
********/
/********
%typemap(out)
  _REAL[3]
{
  $result = PyCapsule_New( $1, NULL, NULL);
}
*********/

%typemap(in) FILE * {
  if ($input == Py_None) {
    $1=NULL;
  } else if (!PyFile_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "Need a file!");
    return NULL;
  } else {
    $1 = PyFile_AsFile($input);
  }
}

%exception
{
  assert( !s_pythread);
  s_pythread = PyThreadState_Get();
  PyEval_ReleaseThread( s_pythread);

  $function

  PyEval_RestoreThread( s_pythread);
  s_pythread = NULL;
}

%typemap(out) char*
{
  if ($1) $result =  PyString_FromString( $1);
  else
    {
      Py_INCREF( Py_None);
      $result = Py_None;
    }
}


enum PyArray_TYPES { PyArray_CHAR, PyArray_UBYTE, PyArray_SBYTE,
			PyArray_SHORT, PyArray_INT, PyArray_LONG,
			PyArray_FLOAT, PyArray_DOUBLE, 
			PyArray_CFLOAT, PyArray_CDOUBLE,
			PyArray_OBJECT,
			PyArray_NTYPES, PyArray_NOTYPE};

%native(createNumArr)PyObject *Py_createNumArr(PyObject *self, PyObject *args);
%{
static PyObject *Py_createNumArr(PyObject *self, PyObject *args)
{
  PyObject * swigPt  = 0 ;
  int dim[1];
  int type;
  void * data;
  PyArrayObject *out;

  if(!PyArg_ParseTuple(args, (char *)"Oii", &swigPt, 
		       &dim[0], &type))
    return NULL;
 
  if (swigPt) 
    {
      /**swig_type_info *ty = SWIG_TypeQuery("void *");
	 if ((SWIG_ConvertPtr(swigPt, (void **) &data, ty, 1 )) == -1) **/
	if ((SWIG_ConvertPtr(swigPt, (void**)&data, 0, 0 )) == -1)
      {
         printf("createNumArr: failed to convert pointer\n");
         return NULL;
      }
    }
  out = (PyArrayObject *)PyArray_FromDimsAndData(1, dim,
						 type, 
						 (char *)data);
  if (!out) 
    {
      PyErr_SetString(PyExc_RuntimeError,
		      "Failed to allocate memory for normals");
      return NULL;
    }

  // so we'll free this memory when this
  // array will be garbage collected
  MGL_SET_OWNDATA(out); 


  return Py_BuildValue("O", (PyObject *)out);
}
%}

%include src/prm.h

parmstruct* parmcalloc( void);
void parmfree( parmstruct* parm);

%{

static PyObject *Py_mme_callback_func[2] = {NULL, NULL};

void sffC_PyCallback(int cbNum, int nbat, _REAL *coords, _REAL *energies, int iter)
{
  PyObject *arglist, *result;
  PyObject * obj0  = 0 ;
  PyObject * obj1  = 0 ;

  assert( s_pythread);
  PyEval_RestoreThread( s_pythread);
  s_pythread = NULL;
  
  obj0 = PyCapsule_New( coords, NULL, NULL);
  obj1 = PyCapsule_New( energies, NULL, NULL);

  arglist = Py_BuildValue("iiOOi", cbNum, nbat, obj0, obj1, iter);
  result = PyObject_CallObject(Py_mme_callback_func[cbNum], arglist);
  if (!result) PyErr_Print();

  Py_XDECREF( result);

  Py_DECREF(obj0);
  Py_DECREF(obj1);
  Py_DECREF(arglist);

  assert( !s_pythread);
  s_pythread = PyThreadState_Get();
  PyEval_ReleaseThread( s_pythread);
  
}

static PyObject *set_callback(PyObject *self, PyObject *args)
{
  PyObject *ofunc;
  int n, freq=1, which;
  char *name;

  if (!PyArg_ParseTuple(args, "Oii", &ofunc, &freq, &which ))
    return NULL;

  if (which<0 || which > NCBFUNC ) {
    PyErr_SetString(PyExc_ValueError,"third argument not valid");
    return NULL;
  }

  mme_callback[which].fun = sffC_PyCallback;

  Py_INCREF(ofunc);
  Py_XDECREF(Py_mme_callback_func[which]);
  Py_mme_callback_func[which] = ofunc;

  mme_callback[which].freq = freq;
  Py_INCREF(Py_None);

  return Py_None;
}


%}

%native (set_callback) set_callback;

%constant mme_f mme_fun = mme;


