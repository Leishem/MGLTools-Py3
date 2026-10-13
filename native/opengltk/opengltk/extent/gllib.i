%module gllib
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
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <pythonplus.h>
%}
#define NUMERIC 
%include glexception.i

%{
static int s_beginlevel = 0;

static PyObject_t gl_begin( PyObject_t self, PyObject_t args)
{
  int mode;

  if(NOT PyArg_ParseTuple(args, "i", &mode)) return NULL;

  if (s_beginlevel) return PyErr_Format( PyExc_RuntimeError,
					 "glBegin() already opened");
  s_beginlevel = 1;
  glBegin( mode);

  s_opengltk->threadunlocked = 0;
  s_opengltk->checkerror = 0;

  return Py_INCREF( Py_None), Py_None;
}

static PyObject_t gl_end( PyObject_t self, PyObject_t args)
{
  GLenum errcode;

  if(NOT PyArg_ParseTuple(args, "")) return NULL;

  if (NOT s_beginlevel) return PyErr_Format( PyExc_RuntimeError,
					     "glBegin() not opened");
  glEnd();

  s_beginlevel = 0;
  s_opengltk->threadunlocked = 1;
  s_opengltk->checkerror = 1;

  errcode = glGetError();
  if (errcode)
    {
      PyObject_t errret;
      errret = s_opengltk->processerror( errcode);
      if (errret) Py_DECREF( errret);
      else return NULL;
    }

  return Py_INCREF( Py_None), Py_None;
}

static PyObject_t stableGetError( PyObject_t self, PyObject_t args)
{
  if(NOT PyArg_ParseTuple(args, "")) return NULL;

  if (NOT s_opengltk->checkerror)
    return PyErr_Format( PyExc_RuntimeError, "NOT s_opengltk->checkerror");

  return PyInt_FromLong( glGetError());
}
%}

%include typemaps.i
%include gltypemap.i 

char const* glGetString( int name);

%native( glBegin) gl_begin;
%native( glEnd) gl_end;
%native( glGetError) stableGetError;

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
%include "gl_i.h"
#endif

#ifdef DEC
#define GL_TEXTURE_1D_BINDING GL_TEXTURE_1D_BINDING_EXT
#define GL_TEXTURE_2D_BINDING GL_TEXTURE_2D_BINDING_EXT
#endif
