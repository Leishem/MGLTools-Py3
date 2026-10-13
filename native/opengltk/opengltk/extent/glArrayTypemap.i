
%init %{
  import_array(); /* load the Numeric PyCObjects */
%}

%{
int checkArraySize = 1;
int checkArgumentsInCWrapper=1;

#include "mgl_npcompat.h"

int NumericTypecode(char *type)
{
  /* maps GL types to Numeric types */
  if (strcmp(type,"GLbyte")==0) return NPY_BYTE;
  else if (strcmp(type,"GLdouble")==0) return NPY_DOUBLE;
  else if (strcmp(type,"GLfloat")==0) return NPY_FLOAT;
  else if (strcmp(type,"GLint")==0) return NPY_INT;
  else if (strcmp(type,"GLshort")==0) return NPY_SHORT;
  else if (strcmp(type,"GLubyte")==0) return NPY_UBYTE;
  else if (strcmp(type,"GLuint")==0) return NPY_INT;
  else if (strcmp(type,"GLushort")==0) return NPY_SHORT;
  else if (strcmp(type,"GLboolean")==0) return NPY_UBYTE;
}

/*********************************************
   1. Check if the incomming object is a buffer object.
   2. If buffer - check if it is a numeric array.
   3. If numeric array -  check if it is contiguous - 
        return 1.
   4. If buffer but not a numeric array - return 1.
   5. In all other cases return 0.
*******************************************/
int isContiguosBuffer(PyObject *incommingObject)
{
  int contig;
  /** Check if object is a buffer object **/
  /** if (PyBuffer_Check(incommingObject)) **/
  PyBufferProcs *pb = incommingObject->ob_type->tp_as_buffer;
  if (pb == NULL || pb->bf_getsegcount == NULL )
    contig = 0;
  else
  {
    /** printf ("object is a buffer\n"); **/
    if (PyArray_Check(incommingObject))
    {
      /** printf ("object is a Numeric array\n"); **/
      if (PyArray_ISCONTIGUOUS((PyArrayObject*)incommingObject))
	contig = 1;
      else contig = 0;	
    }
    else
      contig = 1; 
  }
  return contig;
}
/**********************************************************
This function takes a Python object and creates a Numeric Array 
after checking that the incomming object is of an acceptable
type(size).
***********************************************************/

void bufferWithCheck(PyObject *incommingObject, 
		     PyArrayObject **Narray,
                     char *type, int nbElem)
{
  char buf[255];
  int typecode, size, i;
  /**PyArrayObject *Narray; */
  /* make contiguous if needed */
  typecode = NumericTypecode(type);
  *Narray = (PyArrayObject *)PyArray_ContiguousFromObject(incommingObject,
							typecode, 0, 10 );
  if (*Narray == NULL) 
  {
    sprintf(buf,"Failed to make a contiguous array of type %d\n",
            typecode);
    PyErr_SetString(PyExc_ValueError, buf);
         *Narray =  NULL;
  }
   if (checkArraySize && nbElem)
   {
     /* check size */
     size = 1;
     for (i=0; i<((*Narray)->nd); i++)
     size = size * ((*Narray)->dimensions[i]);
     if (size!=nbElem) 
     {
       sprintf(buf, "%d values received when %d expected\n", size, nbElem);
       PyErr_SetString(PyExc_ValueError, buf);
       *Narray = NULL;
     }
   }
}
%}

extern int checkArgumentsInCWrapper=1;

/**************************************************************/
/* Macro for creating typemaps for:  const GL<type> xxx[ANY]  */
/**************************************************************/

%define IN_GLTYPE_ANY(T)
%typemap(in) const T xxx[ANY] (PyArrayObject *array)
{
  if (checkArgumentsInCWrapper) 
  {
    if (isContiguosBuffer((PyObject*)$input))
    { 
      int buffer_len;
      array = NULL;
      if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
        return NULL;
      if (! $1) return PyErr_Format( PyExc_ValueError,
	 			"NULL buffer not accepted");
    }
    else
    {
      bufferWithCheck($input, &array, "T", $1_dim0);
      if (! array) return NULL;
      $1 = ($1_ltype)array->data;
    }

  }
  else
  {
    int buffer_len;
    array = NULL;
    if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
      return NULL;
    if (! $1) return PyErr_Format( PyExc_ValueError,
				"NULL buffer not accepted");
  }
}

%typemap( freearg) const T xxx[ANY] 
{
  if (array$argnum)
  {
    Py_DECREF(array$argnum);
  }
}
%enddef

/**************************************************************/
/* Macro for creating typemaps for:  const GL<type> *         */
/**************************************************************/

%define IN_GLTYPE(T)
%typemap(in) const T *(PyArrayObject *array)
{
  if (checkArgumentsInCWrapper) 
  {
    if (isContiguosBuffer((PyObject*)$input))
    { 
      int buffer_len;
      array = NULL;
      if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
        return NULL;
      if (! $1) return PyErr_Format( PyExc_ValueError,
	 			"NULL buffer not accepted");
    }
    else
    {
      bufferWithCheck($input, &array, "T", 0);
      if (! array) return NULL;
      $1 = ($1_ltype)array->data;
    }
    /****
    if (array)
    printf ("Array refcnt 1: %d\n", (((PyObject *)array)->ob_refcnt);
    ****/
  }
  else
  {
    int buffer_len;
    array = NULL;
    if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
      return NULL;
    if (! $1) return PyErr_Format( PyExc_ValueError,
				"NULL buffer not accepted");
  }
}

%typemap( freearg) const T * 
{
  if (array$argnum)
  {
    Py_DECREF(array$argnum);
  }
}
%enddef

/** typemap for :
void gluPickMatrix( GLdouble x, GLdouble y, GLdouble delX, 
                    GLdouble delY, GLint xxx[4]);
***/

%typemap(in) GLint xxx[ANY] (PyArrayObject *array)
{
  if (checkArgumentsInCWrapper) 
  {
    if (isContiguosBuffer((PyObject*)$input))
    { 
      int buffer_len;
      array = NULL;
      if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
        return NULL;
      if (! $1) return PyErr_Format( PyExc_ValueError,
	 			"NULL buffer not accepted");
    }
    else
    {
      bufferWithCheck($input, &array, "GLint", $1_dim0);
      if (! array) return NULL;
      $1 = (GLint *)array->data;
    }
    /****
    if (array)
    printf ("Array refcnt 1: %d\n", (((PyObject *)array)->ob_refcnt);
    ****/
  }
  else
  {
    int buffer_len;
    array = NULL;
    if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
      return NULL;
    if (! $1) return PyErr_Format( PyExc_ValueError,
				"NULL buffer not accepted");
  }
}

%typemap( freearg) GLint xxx[ANY] 
{
  if (array$argnum)
  {
    Py_DECREF(array$argnum);
  }
}

/* Generate typemaps */

IN_GLTYPE_ANY(GLbyte)
IN_GLTYPE_ANY(GLdouble)
IN_GLTYPE_ANY(GLfloat)
IN_GLTYPE_ANY(GLint)
IN_GLTYPE_ANY(GLshort)
IN_GLTYPE_ANY(GLubyte)
IN_GLTYPE_ANY(GLuint)
IN_GLTYPE_ANY(GLushort)
IN_GLTYPE_ANY(GLboolean)

IN_GLTYPE(GLbyte)
IN_GLTYPE(GLdouble)
IN_GLTYPE(GLfloat)
IN_GLTYPE(GLint)
IN_GLTYPE(GLshort)
IN_GLTYPE(GLubyte)
IN_GLTYPE(GLuint)
IN_GLTYPE(GLushort)
IN_GLTYPE(GLboolean)
