/*
 * Copyright_notice
 */

%module UTVolumeLibrary

%init %{
	import_array(); /* load the Numeric PyCObjects */
%}
%{

#ifdef _MSC_VER
#include <windows.h>
#define WinVerMajor() LOBYTE(LOWORD(GetVersion()))
#endif

#include "VolumeRenderer.h"
#include "MyExtensions.h"
#include "numpy/arrayobject.h"
#include <GL/gl.h>
#include <string.h>

using namespace OpenGLVolumeRendering;

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


static void InitTexParameteri()
{
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
}

static bool QueryExtension(char *extName)
{
    /*
    ** Search for extName in the extensions string.
    */
    char *p;
    char *end;
    int extNameLen;   

    extNameLen = strlen(extName);
        
    p = (char *)glGetString(GL_EXTENSIONS);
    if (NULL == p) {
        return false;
    }

    end = p + strlen(p);   

    while (p < end) {
        int n = strcspn(p, " ");
        if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
            return true;
        }
        p += (n + 1);
    }
    return false;
}
/***
static bool check_size(int x, int y, int z)
{
  GLint format ;
  glEnable(GL_TEXTURE_3D);
  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_RGBA8, x, y, z, 0, 
	       GL_RGBA, GL_UNSIGNED_BYTE, NULL); 
  glGetTexLevelParameteriv( GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
  if (format) {
    return true;
  } else {
    return false;
  }
}
***/

%}



%include "typemaps.i"
/*****************************
%typemap(python,in) (const GLubyte* data, int width, int height, int depth)(PyArrayObject *array)
{
	if ($input != Py_None)
	{
    		//we only check number of dimesion and not value of each dim
		int expected_dims[3] = {0,0,0};
    		if (expected_dims[0]==1) expected_dims[0]=0;
    		array = contiguous_typed_array($input, PyArray_UBYTE,3, expected_dims);
    		if (! array) return NULL;
		
		$1 = (GLubyte *) array->data;
		$2 = ((PyArrayObject *)(array))->dimensions[0];
		$3 = ((PyArrayObject *)(array))->dimensions[1];
		$4 = ((PyArrayObject *)(array))->dimensions[2];
	 }
  	 else
  	{
		array = NULL;
    		$1 = NULL;
    		$2 = 0;
		$3 = 0;
		$4 = 0;
	}
}
******************/
%typemap(python,in) (const GLubyte* data)(PyArrayObject *array)
{
	if ($input != Py_None)
	{
    		array = contiguous_typed_array($input, PyArray_UBYTE,1, NULL);
    		if (! array) return NULL;
		
		$1 = (GLubyte *) array->data;

	 }
  	 else
  	{
		array = NULL;

	}
}

%typemap(python,in) (const GLubyte* colorMap)(PyArrayObject *array)
{
	
	if ($input != Py_None)
	{
    		/* we only check number of dimesion and not value of each dim*/
		int expected_dims[2] = {256,4};
    		if (expected_dims[0]==1) expected_dims[0]=0;
		if (expected_dims[1]==1) expected_dims[0]=0;
    		array = contiguous_typed_array($input, PyArray_UBYTE,2, expected_dims);
    		if (! array) return NULL;
		
		$1 = (GLubyte *) array->data;
	 }
  	 else
  	{
		array = NULL;
    		$1 = NULL;
	}

}

/* %include VolumeRenderer_i.h */
%include "src/VolumeRenderer.h"

%extend VolumeRenderer {
bool uploadZeroPaddedData(const void* data, int width, int height, int depth) 
{
  return self->uploadColorMappedData((GLubyte*)data, width, height, depth);
}

}

void InitTexParameteri();

bool QueryExtension(char *extName);

//bool check_size(int x, int y, int z);

%native(createNumArr)PyObject *Py_createNumArr(PyObject *self, PyObject *args);
%{
static PyObject *Py_createNumArr(PyObject *self, PyObject *args)
{
  PyObject * swigPt  = 0 ;
  int dim[1];
  unsigned char *pixels;
  PyArrayObject *out;

  if(!PyArg_ParseTuple(args, "Oi", &swigPt, 
		       &dim[0]))
    return NULL;
 
  if (swigPt) 
    {
      //swig_type_info *ty = SWIG_TypeQuery("void *");
      //if ((SWIG_ConvertPtr(swigPt, (void **) &pixels, ty,1)) == -1)
      if ((SWIG_ConvertPtr(swigPt, (void **) &pixels, 0,0)) == -1)
      {
         printf("utvollib, createNumArr: failed to convert a pointer \n");
         return NULL;
      }
    }
  out = (PyArrayObject *)PyArray_FromDimsAndData(1, dim,
						 PyArray_UBYTE, 
						 (char *)pixels);
  if (!out) 
    {
      PyErr_SetString(PyExc_RuntimeError,
		      "Failed to allocate memory for normals");
      return NULL;
    }

#ifdef _MSC_VER
  switch ( WinVerMajor() )
  {
    case 6: break; // Vista
	default: out->flags |= NPY_OWNDATA;
  }
#else
  // so we'll free this memory when this
  // array will be garbage collected
  out->flags |= NPY_OWNDATA; 
#endif

  return Py_BuildValue("O", (PyObject *)out);
}
%}
