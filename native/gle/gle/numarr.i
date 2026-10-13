/*
   This allows to insert the import_array() inti the module initialization
   code. This is required for versions of Python where RTLD+GLOBAL is disabled
   and the Numeric extension is not built into the kernel
*/
%init %{
	import_array(); /* load the Numeric PyCObjects */
%}

%{
#include "numpy/arrayobject.h"

#define Get2D(arr, dims, i, j)        arr[(i * dims[1]) + j]
#define Get3D(arr, dims, i, j, k)     arr[(i * dims[1] * dims[2]) + \
                                                (j * dims[2]) + k]
#define Get4D(arr, dims, i, j, k, l)  arr[(i * dims[1] * dims[2] * dims[3]) + \
                                          (j * dims[2] * dims[3]) +  \
                                          (k * dims[3]) + l]

/********************************************************************
  Tries to create a contiguous numeric array of type typecode from a 
  Python object. Works for list, tuples and numeric arrays.

  obj: Numeric array Python object
  typecode: data type PyArray_{ CHAR, UBYTE, SBYTE, SHORT, INT, LONG, FLOAT,
                                DOUBLE, CFLOAT, CDOUBLE }
  expectnd: required number of dimensions. Used for checking. Ignored if <=0.
  expectdims: array of expected extends. Used for checking. Ignored if <=0.

  Raises ValueError exceptions if:
  - the PyArray_ContiguousFromObject fails
  - the array has a bad shape
  - the extent of a given dimension doesn't match the specified extent.
********************************************************************/

static PyArrayObject *contiguous_typed_array(PyObject *obj, int typecode,
                                      int expectnd, int *expectdims)
{
  PyArrayObject *arr;
  int i, numitems, itemsize;
  char buf[255];

  /* if the shape and type are OK, this function increments the reference
     count and arr points to obj */
  if((arr = (PyArrayObject *)PyArray_ContiguousFromObject(obj,
                                                          typecode, 0,
                                                          10)) == NULL)
    {
      sprintf(buf,"Failed to make a contiguous array of type %d\n", typecode);
      PyErr_SetString(PyExc_ValueError, buf);
      return NULL;
    }

  if(expectnd>0)
    {
      if(arr->nd > expectnd + 1 || arr->nd < expectnd)
        {
          Py_DECREF((PyObject *)arr);
          PyErr_SetString(PyExc_ValueError,
                          "Array has wrong number of dimensions");
          return NULL;
        }
      if(arr->nd == expectnd + 1)
        {
          if(arr->dimensions[arr->nd - 1] != 1)
            {
              Py_DECREF((PyObject *)arr);
              PyErr_SetString(PyExc_ValueError,
                              "Array has wrong number of dimensions");
              return NULL;
            }
        }
      if(expectdims)
        {
          for(i = 0; i < expectnd; i++)
            if(expectdims[i]>0)
              if(expectdims[i] != arr->dimensions[i])
                {
                  Py_DECREF((PyObject *)arr);
                  sprintf(buf,"The extent of dimension %d is %d while %d was expected\n",
                          i, arr->dimensions[i], expectdims[i]);
                  PyErr_SetString(PyExc_ValueError, buf);
                  return NULL;
                }
                  
        }
    }

  return arr;
}

%}

/*******************************************************/
/**              Input: float ARRAY2D_NULL            **/
/*******************************************************/

%typemap(in) float ARRAY2D_NULL[ANY][ANY] (PyArrayObject *array ,
					   int expected_dims[2])

 {
  if ($input == Py_None) {
    $1 = NULL;
  } else {
    expected_dims[0] = $1_dim0;
    expected_dims[1] = $1_dim1;
    if (expected_dims[0]==1) expected_dims[0]=0;
    if (expected_dims[1]==1) expected_dims[1]=0;
    array = contiguous_typed_array($input, PyArray_FLOAT, 2, expected_dims);
    if (! array) return NULL;
    $1 = (float (*)[$1_dim1])array->data;
  }
}

/*******************************************************/
/**              Input: double ARRAY2D_NULL            **/
/*******************************************************/

%typemap(in) double ARRAY2D_NULL[ANY][ANY] (PyArrayObject *array ,
					   int expected_dims[2])

 {
  if ($input == Py_None) {
    $1 = NULL;
  } else {
    expected_dims[0] = $1_dim0;
    expected_dims[1] = $1_dim1;
    if (expected_dims[0]==1) expected_dims[0]=0;
    if (expected_dims[1]==1) expected_dims[1]=0;
    array = contiguous_typed_array($input, PyArray_DOUBLE, 2, expected_dims);
    if (! array) return NULL;
    $1 = (double (*)[$1_dim1])array->data;
  }
}
 
/*************************************************************/
/*                  INPUT: double VECTOR                     */
/*************************************************************/

%typemap(in) double VECTOR[ANY] (PyArrayObject *array, int expected_dims[1])
%{
  expected_dims[0] = $1_dim0;
  if (expected_dims[0]==1) expected_dims[0]=0;
  array = contiguous_typed_array($input, PyArray_DOUBLE, 1, expected_dims);
  if (! array) return NULL;
  $1 = (double *)array->data;
%}

%typemap(freearg) double VECTOR[ANY]

%{
  if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
   
%}

%typemap(in) double VECTOR_NULL[ANY] (PyArrayObject *array) {
  if ($input == Py_None) {
    $1 = NULL;
  } else {
    int expected_dims[1] = {$1_dim0};
    if (expected_dims[0]==1) expected_dims[0]=0;
    array = contiguous_typed_array($input, PyArray_DOUBLE, 1, expected_dims);
    if (! array) return NULL;
    $1 = (double *)array->data;

  }
}


/*************************************************************/
/*                  INPUT: double ARRAY2D                    */
/*************************************************************/

%typemap(in) double ARRAY2D[ANY][ANY](PyArrayObject *array,
			            	  int expected_dims[2]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $1_dim0;
    expected_dims[1] = $1_dim1;
    if (expected_dims[0]==1) expected_dims[0]=0;
    if (expected_dims[1]==1) expected_dims[1]=0;
    array = contiguous_typed_array($input, PyArray_DOUBLE, 2, expected_dims);
    if (! array) return NULL;
    $1 = (double (*)[$1_dim1])array->data;
  }
  else
  {
   array = NULL;
   $1 = NULL;
  }
%}
  
%typemap(freearg) double ARRAY2D[ANY][ANY]
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(python,in) double ARRAY3D_NULL[ANY][ANY][ANY] (PyArrayObject *array) {
  if ($input == Py_None) {
    $1 = NULL;
  } else {
    int expected_dims[3] = {$dim0, $dim1, $dim2};
    if (expected_dims[0]==1) expected_dims[0]=0;
    if (expected_dims[1]==1) expected_dims[1]=0;
    if (expected_dims[2]==1) expected_dims[2]=0;
    array = contiguous_typed_array($input, PyArray_DOUBLE, 3, expected_dims);
    if (! array) return NULL;
  $1 = (double (*)[$1_dim1][$1_dim2])array->data;
  }
}
/**************************************************************/
/*                Input: DOUBLE_ARRAY2D                        */
/**************************************************************/

%define DOUBLE_ARRAY2D( ARRAYNAME, ARRAYSHAPE, DIMENSIONS )
%typemap(in) ( int DIMENSIONS, double ARRAYNAME##ARRAYSHAPE)(PyArrayObject *array, 
                                                        int expected_dims[2])
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $2_dim0;
    expected_dims[1] = $2_dim1;
    if (expected_dims[0]==1) expected_dims[0]=0;
    if (expected_dims[1]==1) expected_dims[1]=0;
    array = contiguous_typed_array($input, PyArray_DOUBLE, 2, expected_dims);
    if (! array) return NULL;
    $2 = (double (*)[$2_dim1])array->data;
    $1 = ((PyArrayObject *)(array))->dimensions[0];
  }
  else
  { 
    array = NULL;
    $2 = NULL;
    $1 = 0;
  }
%}

%typemap(freearg) (int DIMENSIONS, double ARRAYNAME##ARRAYSHAPE) %{
   if (array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%enddef

