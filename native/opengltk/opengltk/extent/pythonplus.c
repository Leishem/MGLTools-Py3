/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is "Java-Python Extension libplus (JPE-libplus)".
 * 
 * The Initial Developer of the Original Code is Frederic Bruno Giacometti.
 * Portions created by Frederic Bruno Giacometti are
 * Copyright (C) 2002 Frederic Bruno Giacometti. All Rights Reserved.
 * 
 * Contributor(s): frederic.giacometti@arakne.com
 *
 * Acknowledgments:
 */

#include <Python.h>
#include <pythonplus.h>

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

PyObject* PypObject_CallMethodArgs( PyObject* self, char const* name,
				    PyObject* args)
{
      PyObject* func, *result = NULL;

      func = PyObject_GetAttrString( self, (char*)name);
      if (! func) {
              PyErr_SetString( PyExc_AttributeError, name);
              return NULL;
      }

      if (!PyCallable_Check( func)) {
              Py_DECREF( func);
              return PypErr_Raise( PyExc_TypeError, "s",
				   "call of non-callable attribute");
      }

      result = PyObject_CallObject( func, args);
      Py_DECREF(func);
      return result;
}

PyObject* PypErr_RaiseArgs( PyObject* exctype, PyObject* args)
{
      PyObject* exception;
      exception = PyObject_CallObject( exctype, args);
      if (! exception) return NULL;
      PyErr_SetObject( exctype, exception);
      return NULL;
}

PyObject* PypErr_Raise( PyObject* exctype, char const* format, ...)
{
      PyObject* args = NULL, *result = NULL;
      va_list va;

      va_start( va, format);
        args = format ? Py_VaBuildValue( (char*)format, va) : PyTuple_New(0);
      va_end(va);

      if (! args) goto Finally;
      if (! PyTuple_Check( args)) {
              PyObject* newargs;
              newargs = PyTuple_New( 1);
              if (! newargs) goto Finally;
              PyTuple_SET_ITEM( newargs, 0, args);
              args = newargs;
      }

      result = PypErr_RaiseArgs( exctype, args);
 Finally:
      Py_XDECREF(args);
      return result;
}

void PypCallback_ProcessErr( char const* cbname)
{
  if (NOT PyErr_Occurred()) return;

  if (PyErr_ExceptionMatches( PyExc_SystemExit))
    {
      PyObject_t exc, val, tb, code;
      int status;

      PyErr_Fetch( &exc, &val, &tb);
      PyErr_NormalizeException( &exc, &val, &tb);
      Py_DECREF( exc); Py_DECREF( tb);

      code = PyObject_GetAttrString( val, "code");
      Py_DECREF( val);

      if (code)
	{
	  status = (code == Py_None ? 0 : PyInt_AsLong( code));
	  if (PyErr_Occurred())
	    {
	      (void)fprintf( stderr, "\n%s:%i python error\n",
			     __FILE__, __LINE__);
	      status = 1;
	      PyErr_Print();
	    }
	  else Py_DECREF( code);
	}
      else
	{
	  (void)fprintf( stderr, "\n%s:%i python error\n", __FILE__, __LINE__);
	  status = 1;
	  PyErr_Print();
	}

      Py_Exit( status);
    }
  else
    {
      (void)fprintf( stderr, "\nUnhandled python exception returned"
		     "to callback <%s>\n", cbname);
      PyErr_Print();
    }
}

int PypObject_SetAttrCobject( PyObject* obj, char const* name,
			      void* ptr, void (*destr)( void*))
{
    PyObject_t cobject;
    int res;

    cobject = PyCObject_FromVoidPtr( ptr, destr);
    if (NOT cobject) return -1;
    res = PyObject_SetAttrString( obj, (char*)name, cobject);
    Py_DECREF( cobject);
    return res;
}


#if PY_VERSION_HEX < 0x01060000

/* pasted from objects/abstract.c in Python source distribution (2.2)
   (C) Python Software Foundation
 */

static PyObject *
null_error()
{
	if (!PyErr_Occurred())
		PyErr_SetString(PyExc_SystemError,
				"null argument to internal routine");
	return NULL;
}

int PyObject_AsCharBuffer(PyObject *obj,
			  const char **buffer,
			  int *buffer_len)
{
	PyBufferProcs *pb;
	const char *pp;
	int len;

	if (obj == NULL || buffer == NULL || buffer_len == NULL) {
		null_error();
		return -1;
	}
	pb = obj->ob_type->tp_as_buffer;
	if (pb == NULL ||
	     pb->bf_getcharbuffer == NULL ||
	     pb->bf_getsegcount == NULL) {
		PyErr_SetString(PyExc_TypeError,
				"expected a character buffer object");
		return -1;
	}
	if ((*pb->bf_getsegcount)(obj,NULL) != 1) {
		PyErr_SetString(PyExc_TypeError,
				"expected a single-segment buffer object");
		return -1;
	}
	len = (*pb->bf_getcharbuffer)(obj, 0, &pp);
	if (len < 0)
		return -1;
	*buffer = pp;
	*buffer_len = len;
	return 0;
}

int
PyObject_CheckReadBuffer(PyObject *obj)
{
	PyBufferProcs *pb = obj->ob_type->tp_as_buffer;

	if (pb == NULL ||
	    pb->bf_getreadbuffer == NULL ||
	    pb->bf_getsegcount == NULL ||
	    (*pb->bf_getsegcount)(obj, NULL) != 1)
		return 0;
	return 1;
}

int PyObject_AsReadBuffer(PyObject *obj,
			  const void **buffer,
			  int *buffer_len)
{
	PyBufferProcs *pb;
	void *pp;
	int len;

	if (obj == NULL || buffer == NULL || buffer_len == NULL) {
		null_error();
		return -1;
	}
	pb = obj->ob_type->tp_as_buffer;
	if (pb == NULL ||
	     pb->bf_getreadbuffer == NULL ||
	     pb->bf_getsegcount == NULL) {
		PyErr_SetString(PyExc_TypeError,
				"expected a readable buffer object");
		return -1;
	}
	if ((*pb->bf_getsegcount)(obj, NULL) != 1) {
		PyErr_SetString(PyExc_TypeError,
				"expected a single-segment buffer object");
		return -1;
	}
	len = (*pb->bf_getreadbuffer)(obj, 0, &pp);
	if (len < 0)
		return -1;
	*buffer = pp;
	*buffer_len = len;
	return 0;
}

int PyObject_AsWriteBuffer(PyObject *obj,
			   void **buffer,
			   long *buffer_len)
{
	PyBufferProcs *pb;
	void*pp;
	long len;

	if (obj == NULL || buffer == NULL || buffer_len == NULL) {
		null_error();
		return -1;
	}
	pb = obj->ob_type->tp_as_buffer;
	if (pb == NULL ||
	     pb->bf_getwritebuffer == NULL ||
	     pb->bf_getsegcount == NULL) {
		PyErr_SetString(PyExc_TypeError,
				"expected a writeable buffer object");
		return -1;
	}
	if ((*pb->bf_getsegcount)(obj, NULL) != 1) {
		PyErr_SetString(PyExc_TypeError,
				"expected a single-segment buffer object");
		return -1;
	}
	len = (*pb->bf_getwritebuffer)(obj,0,&pp);
	if (len < 0)
		return -1;
	*buffer = pp;
	*buffer_len = len;
	return 0;
}

int
PyString_AsStringAndSize(register PyObject *obj,
			 register char **s,
			 register int *len)
{
	if (s == NULL) {
		PyErr_BadInternalCall();
		return -1;
	}

	if (!PyString_Check(obj))
	  {
	    PyErr_Format(PyExc_TypeError,
			 "expected string or Unicode object, "
			 "%.200s found", obj->ob_type->tp_name);
	    return -1;
	  }

	*s = PyString_AS_STRING(obj);
	if (len != NULL)
		*len = PyString_GET_SIZE(obj);
	else if ((int)strlen(*s) != PyString_GET_SIZE(obj)) {
		PyErr_SetString(PyExc_TypeError,
				"expected string without null bytes");
		return -1;
	}
	return 0;
}
#endif /* PY_VERSION_HEX < 0x01060000 */
