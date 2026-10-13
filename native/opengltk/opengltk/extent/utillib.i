/*
 * copyright_notice
 */
%module utillib
%feature("python:cdefaultargs");
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
#include "opengltk.h"

static PyObject_t util_processerror( GLenum errcode)
{
  static PyObject_t glraise = NULL;

  if (NOT glraise)
    {
      PyObject_t excmodule;
      excmodule = PyImport_ImportModule( "opengltk.exception");
      if (NOT excmodule) return NULL;
      glraise = PyObject_GetAttrString( excmodule, "processglerror");
      Py_DECREF( excmodule);
      if (NOT glraise) return NULL;
    }

  return PyObject_CallFunction( glraise, "i", errcode);
}

static struct opengltk_export utilexport = {
  util_processerror,
  NULL, /*pythread*/
  1, /*threadunlocked*/
  1 /*checkerror*/
};

static PyObject_t attachCurrentThread( PyObject_t self, PyObject_t args)
{
  if (NOT PyArg_ParseTuple( args, "")) return NULL;

  if (utilexport.pythread)
    {
      if (PyThreadState_Get() == utilexport.pythread)
	return PyErr_Format( PyExc_RuntimeError,
			     "current thread already attached");
      else PyErr_Format( PyExc_RuntimeError,
			 "already attached to a different thread");
    }
  utilexport.pythread = PyThreadState_Get();

  return Py_INCREF( Py_None), Py_None;
}

static PyObject_t detachCurrentThread( PyObject_t self, PyObject_t args)
{
  if (NOT PyArg_ParseTuple( args, "")) return NULL;

  if (NOT utilexport.pythread)
    return PyErr_Format( PyExc_RuntimeError, "thread not attached");

  if (PyThreadState_Get() != utilexport.pythread)
    return PyErr_Format( PyExc_RuntimeError,
			 "thread attached to a different thread");
  utilexport.pythread = NULL;

  return Py_INCREF( Py_None), Py_None;
}

static PyObject_t attachedThread( PyObject_t self, PyObject_t args)
{
  if (NOT PyArg_ParseTuple( args, "")) return NULL;

  return PyErr_Format( PyExc_NotImplementedError, __FILE__ ":%i", __LINE__);
  /* return utilexport.pythread ?
     /x how does one creat the Thread object
       from a PyThreadState pointer.... ??? x/
       : Py_INCREF( Py_None), Py_None; */
}
%}

%include typemaps.i
//%include pointer.i
%include callback.i

%native( attachCurrentThread) attachCurrentThread;
%native( detachCurrentThread) detachCurrentThread;
%native( attachedThread) attachedThread;

//%rename( checkGLerror) utilexport.checkerror; //does not work on swig 1.3.11???

%constant int sizeof_GLbitfield = sizeof (GLbitfield);
%constant int sizeof_GLboolean = sizeof (GLboolean);
%constant int sizeof_GLbyte = sizeof (GLbyte);
%constant int sizeof_GLclampd = sizeof (GLclampd);
%constant int sizeof_GLclampf = sizeof (GLclampf);
%constant int sizeof_GLdouble = sizeof (GLdouble);
%constant int sizeof_GLenum = sizeof (GLenum);
%constant int sizeof_GLfloat = sizeof (GLfloat);
%constant int sizeof_GLint = sizeof (GLint);
%constant int sizeof_GLshort = sizeof (GLshort);
%constant int sizeof_GLsizei = sizeof (GLsizei);
%constant int sizeof_GLubyte = sizeof (GLubyte);
%constant int sizeof_GLuint = sizeof (GLuint);
%constant int sizeof_GLushort = sizeof (GLushort);

%init
{
  if (PypObject_SetAttrCobject( m, "opengltk_export",
				&utilexport, NULL)) return;
  
}

%include dejavu.i
