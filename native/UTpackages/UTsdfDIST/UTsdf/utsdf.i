%module utsdf

%init %{
  import_array(); /* load the Numeric PyCObjects */
%}

%{

#ifdef _MSC_VER
#include <windows.h>
#define WinVerMajor() LOBYTE(LOWORD(GetVersion()))
#endif

#include "mgl_npcompat.h"

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
%{
#include "sdfLib.h"
%}


// typemap to input an array of verts and triangles
%typemap(in) (int nverts, float* verts) (PyArrayObject *array, 
					 int expected_dims[2]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0] = 0;
    expected_dims[1] = 3;		
    array = contiguous_typed_array($input, PyArray_FLOAT, 2, expected_dims);
    if (! array) return NULL;
    $1 = ((PyArrayObject *)(array))->dimensions[0];
    $2 = (float *)array->data;
    
  }
  else
  {
    array = NULL;
    $1 = 0;
    $2 = NULL;
  }
%}

%typemap(freearg) (int nverts, float* verts)
%{
   if (array$argnum)
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(in) (int ntris, int* tris) (PyArrayObject *array, 
					 int expected_dims[2]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0] = 0;
    expected_dims[1] = 3;		
    array = contiguous_typed_array($input, PyArray_INT, 2, expected_dims);
    if (! array) return NULL;
    $1 = ((PyArrayObject *)(array))->dimensions[0];
    $2 = (int *)array->data;
    
  }
  else
  {
    array = NULL;
    $1 = 0;
    $2 = NULL;
    
  }
%}

%typemap(freearg) (int ntris, int* tris)
%{
   if (array$argnum)
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(in) double bufarr[6] (PyArrayObject *array, int expected_dims[1])
%{
  expected_dims[0] = $1_dim0;
  if (expected_dims[0]==1) expected_dims[0]=0;
  array = contiguous_typed_array($input, PyArray_DOUBLE, 1, expected_dims);
  if (! array) return NULL;
  $1 = (double *)array->data;
%}

%typemap(freearg) double bufarr[6]

%{
  if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
   
%}


%native(createNumArr)PyObject *Py_createNumArr(PyObject *self, PyObject *args);
%{
static PyObject *Py_createNumArr(PyObject *self, PyObject *args)
{
  PyObject * swigPt  = 0 ;
  int dim[1];
  float *data;
  PyArrayObject *out;

  if(!PyArg_ParseTuple(args, "Oi", &swigPt, 
		       &dim[0]))
    return NULL;
 
  if (swigPt) 
    {
        swig_type_info *ty = SWIG_TypeQuery("float *");
        if ((SWIG_ConvertPtr(swigPt, (void **)&data, ty,1)) == -1)
      {
         printf("utsdf,createNumArr: failed to convert a pointer\n");
         return NULL;
      }
    }
  out = (PyArrayObject *)PyArray_FromDimsAndData(1, dim,
						 PyArray_FLOAT, 
						 (char *) data);
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

%include src/sdfLib.h 
