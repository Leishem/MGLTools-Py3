%module geomalgorithms
%feature("python:cdefaultargs");

%include geomutils.i
%{
#include "distance.h"
#include "intersections.h"
#include "polygonArea.h"
#include "boundingContainers.h"
#include "objfile.h"
%}


//%include numarr.i
//typemap for arguments in  closest 2D_Point_to_Line()
%typemap(in) (Point P[], int n) (PyArrayObject *array = NULL, int dims[2], char *cdata)
{
  array = (PyArrayObject *)PyArray_ContiguousFromObject($input, PyArray_DOUBLE, 2, 2);
  if (array ==NULL)
    {
      PyErr_SetString(PyExc_ValueError, "Failed to create 2D contiguous array of type double");
      return NULL;
    }
  dims[0] = array->dimensions[0];
  dims[1] = array->dimensions[1];
  cdata = array->data;
  if(dims[1] != 3 && dims[1] != 2)
    {
      PyErr_SetString(PyExc_ValueError, "Wrong shape of the input array; Expected (n,3) or (n,2)");
      return NULL;
    }
  $1 = (Point *)malloc(dims[0]*sizeof(Point));
  for (int i =0; i< dims[0]; i++)
    {
      double x, y,z;
      x =  *(double *)(cdata + i*array->strides[0]);
      y = *(double *)(cdata + i*array->strides[0] + array->strides[1]);
      if (dims[1] ==2)
	{
	  z = 0;
	  $1[i] = Point(x, y);
	}
      else // dims[1] ==3 
	{
	  z = *(double *)(cdata + i*array->strides[0] + 2*array->strides[1]);
	  $1[i] = Point(x, y, z);
	}
      //printf ("point [%d] = (%f, %f, %f )\n", i, x, y, z);
      $2 = dims[0];
    }
}

%typemap(freearg)(Point P[], int n) 
{
  if (array$argnum )
    Py_DECREF((PyObject *)array$argnum);
  if ($1)
    free($1);
}


//typemap for Point to input list [x,y], or list [x,y,z] from Python
%typemap (in) Point (PyObject *o = NULL , double temp[3] = {0.0, 0.0, 0.0}, int dim )	

{
  if (!PyList_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expecting a list");
      return NULL;
    }
  dim = PySequence_Length($input);
  if (dim != 3 && dim != 2) 
    {
      PyErr_SetString(PyExc_ValueError,"Wrong list size; Expected 2 or 3 elements");
      return NULL;
    }
  for (int i = 0; i < dim; i++)
    {
      o = PyList_GetItem($input,i);
      if (PyNumber_Check(o)) 
	temp[i] = (double) PyFloat_AsDouble(o);
      else 
	{
	  PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
	  return NULL;
	}
    }
  if (dim == 2)
    {
      $1.x = temp[0];
      $1.y = temp[1];
    }
  else // dim == 3 
    {
      $1.x = temp[0];
      $1.y = temp[1];
      $1.z = temp[2];
    }
      $1.setdim(dim);
}

//typemap for Line , Segment, Ray to input list [[x, y], [x,y]] or [[x,y,z], [x,y,z]]

%typemap (in) Line (PyObject *pyobj = NULL, PyObject *item = NULL, Point P[2], 
                    double temp[3] = {0.0, 0.0, 0.0}, int dim)
{
  if (!PyList_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expecting a list of 2 lists");
      return NULL;
    }
  if( PySequence_Length($input) != 2)
    {
      PyErr_SetString(PyExc_ValueError,"Wrong list size; Expected a list of 2 lists");
      return NULL;
    }
  for (int i = 0; i < 2; i++)
    {
      pyobj = PyList_GetItem($input, i);
      if ( !PyList_Check(pyobj))
	{
	  PyErr_SetString(PyExc_ValueError, "Expecting a list of 2 lists");
	  return NULL;
	}
      dim = PySequence_Length(pyobj);
      if (dim != 3 && dim != 2) 
	{
	  PyErr_SetString(PyExc_ValueError,"Wrong list size; Expected 2 or 3 elements");
	  return NULL;
	}
      for (int j = 0; j < dim; j++)
	{
	  item = PyList_GetItem(pyobj, j);
	  temp[j] = (double) PyFloat_AsDouble(item);
	}
      if (dim == 2)
	  P[i] = Point(temp[0], temp[1]);
      else // dim == 3 
	  P[i]= Point(temp[0], temp[1], temp[2]);
    }
      
  $1.P0 = P[0];
  $1.P1 = P[1];
}
%apply Line {Segment, Ray};

//typemap for  Point* to output a list [x,y,z] from Python
// (in 2D case z = 0.0)
//%include "fragments.i"

%typemap(in, numinputs=0) Point* (Point point)
 {
   $1 = &point;
 }

//%typemap(argout,fragment="t_output_helper" ) Point* (PyObject *newlist= NULL)
%typemap(argout ) Point* (PyObject *newlist= NULL)
{
  newlist = PyList_New(3);
  PyList_SetItem(newlist, 0, PyFloat_FromDouble($1->x) );
  PyList_SetItem(newlist, 1, PyFloat_FromDouble($1->y) );
  if ($1->dim()==3)
    PyList_SetItem(newlist, 2, PyFloat_FromDouble($1->z) );
  else // $1->dim()== 2
    PyList_SetItem(newlist, 2, PyFloat_FromDouble(0.0) );
  $result = t_output_helper2($result, newlist);
}


// typemap for argument Triangle to input a sequence : [[x1,y1,z1],[x2,y2,z2],[x3,y3,z3]] 
%typemap (in) Triangle (PyObject *pyobj = NULL, PyObject *item = NULL, Point P[3], 
                    double temp[3] = {0.0, 0.0, 0.0}, int dim)
{
  if (!PySequence_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expecting a sequence: [[x1,y1,z1],[x2,y2,z2],[x3,y3,z3]]");
      return NULL;
    }

  if( PySequence_Length($input) != 3)
    {
      PyErr_SetString(PyExc_ValueError,"Wrong sequence size; Expecting (3, 3)");
      return NULL;
    }
  for (int i = 0; i < 3; i++)
    {
      pyobj = PySequence_GetItem($input, i);
      if ( !PySequence_Check(pyobj))
	{
	  PyErr_SetString(PyExc_ValueError, "Expecting a sequence: [x, y, z]");
	  return NULL;
	}
      dim = PySequence_Length(pyobj);
      if (dim != 3) 
	{
	  PyErr_SetString(PyExc_ValueError,"Wrong sequence size; Expecting (3, 3)");
	  return NULL;
	}
      for (int j = 0; j < dim; j++)
	{
	  item = PyList_GetItem(pyobj, j);
	  temp[j] = (double) PyFloat_AsDouble(item);
	}

      P[i]= Point(temp[0], temp[1], temp[2]);
    }
      
  $1.V0 = P[0];
  $1.V1 = P[1];
  $1.V2 = P[2];
}

//typemap for argument Plane={Point V0; Vector n;}
// to input a list [[x1, y1, z1], [x2, y2, z2]] in Python
 
%typemap (in) Plane (PyObject *pyobj = NULL, PyObject *item = NULL, 
                    double coords[2][3]={{.0, .0, .0}, {.0,.0,.0}}, int dim)
{
  if (!PyList_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expected a list: [[x1, y1, z1], [x2, y2, z2]] ");
      return NULL;
    }
  if( PySequence_Length($input) != 2)
    {
      PyErr_SetString(PyExc_ValueError,"Wrong list size. Expected a list of 2 lists; ");
      return NULL;
    }
  for (int i = 0; i < 2; i++)
    {
      pyobj = PyList_GetItem($input, i);
      if ( !PyList_Check(pyobj))
	{
	  PyErr_SetString(PyExc_ValueError, "Wrong input type. Expected a list of 2 lists");
	  return NULL;
	}
      dim = PySequence_Length(pyobj);
      if (dim != 3 && dim != 2) 
	{
	  PyErr_SetString(PyExc_ValueError,"Wrong list size; Expected 2 or 3 elements");
	  return NULL;
	}
      for (int j = 0; j < dim; j++)
	{
	  item = PyList_GetItem(pyobj, j);
	  coords[i][j] = (double) PyFloat_AsDouble(item);
	}
    }

  $1.V0 = Point(coords[0][0], coords[0][1], coords[0][2]);
  $1.n = Vector(coords[1][0], coords[1][1], coords[1][2]);
}

//typemap for  Line* to output a list [[x1,y1,z1], [x2, y2, z2]] from Python
// (in 2D case z = 0.0)
//%include "fragments.i"
%typemap(in, numinputs=0) Line* (Line line)
 {
   $1 = &line;
 }

//%typemap(argout,fragment="t_output_helper" ) Line* (PyObject * l, PyObject * ll, Point PP[2])
%typemap(argout ) Line* (PyObject * l, PyObject * ll, Point PP[2])
{
  l = PyList_New(2);
  ll = PyList_New(3);
  
  PP[0] = $1->P0; PP[1] = $1->P1;
  for (int i = 0; i<2; i++)
    {
      PyList_SetItem(ll, 0, PyFloat_FromDouble(PP[i].x) );
      PyList_SetItem(ll, 1, PyFloat_FromDouble(PP[i].y) );
      PyList_SetItem(ll, 2, PyFloat_FromDouble(PP[i].z) );

      PyList_SetItem(l, i, ll);
    }
  $result = t_output_helper2($result, l);
}

//typemap for multiple arg list (int n, Vert* V) to input a numpy array of vertices.
//  V is an array of n+1 vertices with V[n] = V[0] (see polygonArea.h for deatails)

%typemap(in) (int n, Vert* V) ( PyArrayObject *array, int arrdims[2], char *cdata)
 {
  array = (PyArrayObject *)PyArray_ContiguousFromObject($input, PyArray_DOUBLE, 2, 2);
  if (array ==NULL)
    {
      PyErr_SetString(PyExc_ValueError, "Failed to create 2D contiguous array of type double");
      return NULL;
    }
  arrdims[0] = array->dimensions[0];
  arrdims[1] = array->dimensions[1];
  cdata = array->data;
  if(arrdims[1] != 3)
    {
      PyErr_SetString(PyExc_ValueError, "Wrong shape of the input array; Expected (n,3).");
      return NULL;
    }
  $2 = (Vert *)malloc(arrdims[0]*sizeof(Vert));
  for (int i =0; i< arrdims[0]; i++)
    {
      double x, y,z;
      x =  *(double *)(cdata + i*array->strides[0]);
      y = *(double *)(cdata + i*array->strides[0] + array->strides[1]);
      z = *(double *)(cdata + i*array->strides[0] + 2*array->strides[1]);
      $2[i].x = x; $2[i].y = y; $2[i].z = z;

      //printf ("vert[%d] = (%f, %f, %f )\n", i, x, y, z);
      $1 = arrdims[0];
    }
 }
%typemap(freearg)(int n, Vert* V)
{
  if (array$argnum )
    Py_DECREF((PyObject *)array$argnum);
  if ($2)
    free($2);
}


%typemap (in) Vert (PyObject *o = NULL , double temp[3] = {0.0, 0.0, 0.0}, int dim )	

{
  if (!PyList_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expecting a list");
      return NULL;
    }
  dim = PySequence_Length($input);
  if (dim != 3) 
    {
      PyErr_SetString(PyExc_ValueError,"Wrong list size; Expected 2 or 3 elements");
      return NULL;
    }
  for (int i = 0; i < dim; i++)
    {
      o = PyList_GetItem($input,i);
      if (PyNumber_Check(o)) 
	temp[i] = (double) PyFloat_AsDouble(o);
      else 
	{
	  PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
	  return NULL;
	}
    }
      $1.x = temp[0];
      $1.y = temp[1];
      $1.z = temp[2];

}


%include src/geomAlgorithms/distance.h
%include src/geomAlgorithms/intersections.h
%include src/geomAlgorithms/polygonArea.h
%include src/geomAlgorithms/boundingContainers.h 

%include typemaps.i


/* *********************** start of readObjFile *********************************** */

%typemap(in) (char aoGroupNames[][256], unsigned int * aiNumOfGroupNames)(PyArrayObject *array=NULL,unsigned int dd)
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_CHAR, 2, NULL);
    if (! array) return NULL;
    $1 = (char (*)[$1_dim1])array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (char aoGroupNames[][256], unsigned int * aiNumOfGroupNames)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(char aoGroupNames[][256], unsigned int * aiNumOfGroupNames) (PyObject * intobj)
%{
   
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result, intobj);
%}


%typemap(in) (float aoVertices[][3], unsigned int * aiNumOfVertices)(PyArrayObject *array=NULL,unsigned int dd)
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_FLOAT, 2, NULL);
    if (! array) return NULL;
    $1 = (float (*)[$1_dim1])array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (float aoVertices[][3], unsigned int * aiNumOfVertices)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(float aoVertices[][3], unsigned int * aiNumOfVertices) (PyObject * intobj)
%{
   
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result, intobj);
%}


%typemap(in) (int aoTriangles[][3], unsigned int * aiNumOfTriangles)(PyArrayObject *array=NULL,unsigned int dd )
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_INT, 2, NULL);
    if (! array) return NULL;
    $1 = (int (*)[$1_dim1])array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (int aoTriangles[][3], unsigned int * aiNumOfTriangles)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(int aoTriangles[][3], unsigned int * aiNumOfTriangles)(PyObject * intobj)
%{
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result,intobj);
%}


%typemap(in) (float aoTextureVertices[][2], unsigned int * aiNumOfTextureVertices)(PyArrayObject *array=NULL,unsigned int dd)
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_FLOAT, 2, NULL);
    if (! array) return NULL;
    $1 = (float (*)[$1_dim1])array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (float aoTextureVertices[][3], unsigned int * aiNumOfTextureVertices)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(float aoTextureVertices[][2], unsigned int * aiNumOfTextureVertices) (PyObject * intobj)
%{
   
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result, intobj);
%}


%typemap(in) (int aoTextureTriangles[][3], unsigned int * aiNumOfTextureTriangles)(PyArrayObject *array=NULL,unsigned int dd )
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_INT, 2, NULL);
    if (! array) return NULL;
    $1 = (int (*)[$1_dim1])array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (int aoTextureTriangles[][3], unsigned int * aiNumOfTextureTriangles)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(int aoTextureTriangles[][3], unsigned int * aiNumOfTextureTriangles)(PyObject * intobj)
%{
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result,intobj);
%}


%typemap(in) (char aoMaterialLibraries[][256], unsigned int * aiNumOfMaterialLibraries)(PyArrayObject *array=NULL,unsigned int dd)
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_CHAR, 2, NULL);
    if (! array) return NULL;
    $1 = (char (*)[$1_dim1])array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (char aoMaterialLibraries[][256], unsigned int * aiNumOfMaterialLibraries)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(char aoMaterialLibraries[][256], unsigned int * aiNumOfMaterialLibraries) (PyObject * intobj)
%{
   
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result, intobj);
%}


%typemap(in) (char aoMaterialNames[][256], unsigned int * aiNumOfMaterialNames)(PyArrayObject *array=NULL,unsigned int dd)
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_CHAR, 2, NULL);
    if (! array) return NULL;
    $1 = (char (*)[$1_dim1])array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (char aoMaterialNames[][256], unsigned int * aiNumOfMaterialNames)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(char aoMaterialNames[][256], unsigned int * aiNumOfMaterialNames) (PyObject * intobj)
%{
   
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result, intobj);
%}


%typemap(in) (int aoTriangleMaterialIndices[], unsigned int * aiNumOfTriangleMaterialIndices)(PyArrayObject *array=NULL,unsigned int dd )
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_INT, 1, NULL);
    if (! array) return NULL;
    $1 = (int *)array->data;
    dd = (unsigned int)((PyArrayObject *)(array))->dimensions[0];
    $2 = &dd;
  }
  else
  {
   array = NULL;
   $1 = NULL;
   dd = 0;
   $2 = &dd;
  }

%}

%typemap(freearg) (int aoTriangleMaterialIndices[], unsigned int * aiNumOfTriangleMaterialIndices)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(argout)(int aoTriangleMaterialIndices[], unsigned int * aiNumOfTriangleMaterialIndices)(PyObject * intobj)
%{
   intobj = PyInt_FromLong((long)(*$2));
   $result = l_output_helper2($result,intobj);
%}



%include src/geomAlgorithms/objfile.h

/* *********************** end of readObjFile *********************************** */

