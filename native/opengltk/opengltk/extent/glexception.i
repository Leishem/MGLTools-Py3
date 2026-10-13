/*
 * copyright_notice
 */

%{
#include "opengltk.h"

static struct opengltk_export* s_opengltk = NULL;
PyObject_t PypImport_ModuleAttr( char const* modulename, char const* name)
{
  PyObject_t module, result;
  module = PyImport_ImportModule( (char*)modulename);
  if (NOT module) return NULL;
  result = PyObject_GetAttrString( module, (char*)name);
  Py_DECREF( module);
  return result;
}

void* PypImport_ModuleCobjAttr( char const* module, char const* name)
{
  PyObject_t attr;
  void* result;

  attr = PypImport_ModuleAttr( module, name);
  if (NOT attr) return NULL;
  result = PyCapsule_GetPointer( attr, NULL);
  Py_DECREF( attr);
  return result;
}


%}

%init
{
  s_opengltk = PypImport_ModuleCobjAttr( "opengltk.extent._utillib",
					 "opengltk_export");
  if (NOT s_opengltk) return;
  s_opengltk->checkerror = 1;
}

%exception
{
  GLenum errcode;

  if (s_opengltk->pythread)
    {
      printf ("pythread \n");
      if (PyThreadState_Get() != s_opengltk->pythread)
	return PyErr_Format( PyExc_RuntimeError,
			     "OpenGL is attached to a different thread");
      PyEval_ReleaseThread( s_opengltk->pythread);
    }
  $function
  if (s_opengltk->checkerror)
	
    {	
      errcode = glGetError();
      if (s_opengltk->pythread && s_opengltk->threadunlocked)
	PyEval_RestoreThread( s_opengltk->pythread);
#ifdef _WIN32
/*       if (GL_INVALID_OPERATION == errcode) printf( "rrrrrrr\n"); */
      if (errcode && GL_INVALID_OPERATION != errcode)
#else
      if (errcode)
#endif
	{
	  PyObject_t errret;
	  errret = s_opengltk->processerror( errcode);
	  if (errret) Py_DECREF( errret);
	  else return NULL;
	}
    }
  else if (s_opengltk->pythread && s_opengltk->threadunlocked)
    PyEval_RestoreThread( s_opengltk->pythread);
}
