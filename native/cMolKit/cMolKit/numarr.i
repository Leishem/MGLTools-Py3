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


static PyArrayObject *contiguous_typed_array(PyObject *obj, int typecode,
                                      int expectnd, int *expectdims)
{
  PyArrayObject *arr;
  int i;
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


%define INT_VECTOR( ARRAYNAME, ARRAYSHAPE, LENGTH )
%typemap(in) (int ARRAYNAME##ARRAYSHAPE, int LENGTH) (PyArrayObject *array, 
						      int expected_dims[1]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $1_dim0;
    if (expected_dims[0]==1) expected_dims[0]=0;
    array = contiguous_typed_array($input, PyArray_INT, 1, expected_dims);
    if (! array) return NULL;
    $1 = (int *)array->data;
    $2 = ((PyArrayObject *)(array))->dimensions[0];
  }
  else
  {
    array = NULL;
    $1 = NULL;
    $2 = 0;
  }
%}
%typemap(freearg) (int ARRAYNAME##ARRAYSHAPE, int LENGTH) %{
  if (array$argnum)
    {
      Py_DECREF((PyObject *)array$argnum);
    }
%}
%enddef

%define INT_ARRAY2D( ARRAYNAME, ARRAYSHAPE, DIMENSIONS )
%typemap(in) ( int ARRAYNAME##ARRAYSHAPE,  int DIMENSIONS)(PyArrayObject *array, 
						       int expected_dims[2])
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $1_dim0;
    expected_dims[1] = $1_dim1;
    if (expected_dims[0]==1) expected_dims[0]=0;
    if (expected_dims[1]==1) expected_dims[1]=0;
    array = contiguous_typed_array($input, PyArray_INT, 2, expected_dims);
    if (! array) return NULL;
    $1 = (int (*)[$1_dim1])array->data;
    $2 = ((PyArrayObject *)(array))->dimensions[0];
  }
  else
  {
    array = NULL;
    $1 = NULL;
    $2 = 0;
  }
%}

%typemap(freearg) (int ARRAYNAME##ARRAYSHAPE, int DIMENSIONS) %{
   if (array$argnum)
     {
       Py_DECREF((PyObject *)array$argnum);
     }
%}

%enddef


%define FLOAT_ARRAY2D( ARRAYNAME, ARRAYSHAPE, DIMENSIONS )
%typemap(in) ( float ARRAYNAME##ARRAYSHAPE,  int DIMENSIONS)(PyArrayObject *array, 
                                                        int expected_dims[2])
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $1_dim0;
    expected_dims[1] = $1_dim1;
    if (expected_dims[0]==1) expected_dims[0]=0;
    if (expected_dims[1]==1) expected_dims[1]=0;
    array = contiguous_typed_array($input, PyArray_FLOAT, 2, expected_dims);
    if (! array) return NULL;
    $1 = (float (*)[$1_dim1])array->data;
    $2 = ((PyArrayObject *)(array))->dimensions[0];
  }
  else
  { 
    array = NULL;
    $1 = NULL;
    $2 = 0;
  }
%}

%typemap(freearg) (float ARRAYNAME##ARRAYSHAPE, int DIMENSIONS ) %{
   if (array$argnum )
     {
      Py_DECREF((PyObject *)array$argnum);
     }
%}

%enddef


