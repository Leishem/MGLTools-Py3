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
 * Copyright (C) 2001-2002 Frederic Bruno Giacometti. All Rights Reserved.
 * 
 * Contributor(s): frederic.giacometti@arakne.com
 *
 * Acknowledgments:
 */

#include <Python.h>
typedef PyObject* PyObject_t;

#include <utilplus.h>

#define PypBuffer_Check( obj) (NOT NOT (obj)->ob_type->tp_as_buffer)

typedef long idx_v;

#define PypExport_TypeEqual( export, obj) ((export)->type == (obj)->ob_type)
#define PypLExport_TypeEqual( export, obj) ((export).type == (obj)->ob_type)

PyObject_t PypImport_ModuleAttr( char const* module, char const* name);
void* PypImport_ModuleCobjAttr( char const* module, char const* name);

PyObject* PypObject_CallMethodArgs( PyObject* self, char const* name,
				    PyObject* args);
PyObject* PypErr_RaiseArgs( PyObject* exctype, PyObject* args);
PyObject* PypErr_Raise( PyObject* exctype, char const* format, ...);

void PypCallback_ProcessErr( char const* cbname);

int PypObject_SetAttrCobject( PyObject* obj, char const* name,
			      void* ptr, void (*destr)( void*));


#if PY_VERSION_HEX < 0x01060000

/* pasted from objects/abstract.c in Python source distribution (2.2)
   (C) Python Software Foundation
 */
int PyObject_AsCharBuffer(PyObject *obj,
			  const char **buffer,
			  int *buffer_len);
int PyObject_CheckReadBuffer(PyObject *obj);

int PyObject_AsReadBuffer(PyObject *obj,
			  const void **buffer,
			  int *buffer_len);

int PyObject_AsWriteBuffer(PyObject *obj,
			   void **buffer,
			   long *buffer_len);

int
PyString_AsStringAndSize(PyObject *obj,
			 char **s,
			 int *len);

#endif /* PY_VERSION_HEX < 0x01060000 */


#define PYTHONPLUS_H
