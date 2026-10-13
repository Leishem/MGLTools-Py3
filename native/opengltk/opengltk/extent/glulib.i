%module glulib 
%feature("python:cdefaultargs");
/*
 * copyright_notice
 */

%{
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
# include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <pythonplus.h>
#include "callback.h"

static PyObject_t glu_error_string( PyObject_t self, PyObject_t args)
{
  int errcode;
  char const* result;

  if(NOT PyArg_ParseTuple(args, "i", &errcode)) return NULL;

  result = (char const*)gluErrorString( (GLenum)errcode);
  
  return result ? PyString_InternFromString( result)
    : (Py_INCREF( Py_None), Py_None);
}
%}


%include typemaps.i
%include gltypedef.i
%include callback.h
%include gltypemap.i
%include glexception.i

#ifdef GLMesa
    void gluNurbsCallback( GLUnurbsObj*, GLenum, void_GLenum_f);
    void gluQuadricCallback( GLUquadricObj* quad, GLenum which, void_GLenum_f);
#else
    void gluNurbsCallback( GLUnurbs*, GLenum, void_GLenum_f);
    void gluQuadricCallback( GLUquadric* quad, GLenum which, void_GLenum_f);
    void gluTessCallback( GLUtesselator* tess, GLenum which, void_GLenum_f);
#endif


%native( gluErrorString) glu_error_string;

%include "glu_i.h"
