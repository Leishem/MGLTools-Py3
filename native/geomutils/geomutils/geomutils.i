%feature ("kwargs");
%{
  //#include <stdio.h>
  //#include <string.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>

#ifdef __APPLE__
#undef max
#endif


using namespace std;
/****************************************************************************
  Compute vector v, normal to the triangle (p1,p2,p3) assuming that the order
  of the points p1,p2,p3 provides the face's orientation
*****************************************************************************/
static void triangle_normal(double *p1,double *p2, double *p3, float *v)
{
    double v1[3],v2[3],norm;
    short i;

    for (i=0; i<3; i++) {
      v1[i] = p2[i]-p1[i];   /* vector (p1,p2) */
      v2[i] = p3[i]-p2[i];   /* vector (p2,p3) */
    }
    v[0] = v1[1]*v2[2] - v1[2]*v2[1];  /* v3 = v1^v2 */
    v[1] = v1[2]*v2[0] - v1[0]*v2[2];
    v[2] = v1[0]*v2[1] - v1[1]*v2[0];

    norm = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (norm != 0.) {
      for (i=0;i<3;i++) v[i] /= norm;
    } else {
      for (i=0;i<3;i++) v[i] = 0.0;
    }
}


/********************************************************
 Computes the vector normal to each triangle using
 triangle_normal. The resulting normals are returned
 in a m*3 array of floats.
********************************************************/

int triangleNormalsPerFace(double *v_data, int lenv[2],  
			    int *t_data, int lent[2],
			    float *trinorm)
{
  int   i;
  for (i=0; i<3*lent[0]; i+=3) 
    {
      int v1,v2,v3;
      v1 = t_data[i];
      if ( v1 >= lenv[0] ) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range\n", v1, i/3);
	  return 0;
	}
      v2 = t_data[i+1];
      if ( v2 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range\n", v1, i/3);
	  return 0;
	}
      v3 = t_data[i+2];
      if ( v3 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range\n", v3, i/3);
	  return 0;
	}
      
      triangle_normal( &v_data[v1*3], &v_data[v2*3], &v_data[v3*3], 
		       &trinorm[i] );
    }
  return 1;
}

/**************************************************************************
Computes the vector normal to each triangle (face) using
triangle_normal. The normals for each vertex are obtained by 
summing up the faces normals of each triangle this vertex belongs to. The 
resulting normals are returned in a n*3 array of floats.
***************************************************************************/
int triangleNormalsPerVertex(double *v_data, int lenv[2], float *vnorm,
			      int *t_data, int lent[2])
{
  int   i, j, k, *tric;
  float *trinorm;
  /*trinorm = (float *)malloc(lent[0] * 3 * sizeof(float)); */
  trinorm = (float *)malloc(lent[0] * lent[1] * sizeof(float));
  if (!trinorm)
    {
      fprintf(stderr, "Failed to allocate memory for the triangle normals \n");
      return 0;
    }
  for (i=0; i<3*lent[0]; i+=3) 
    {
      int v1,v2,v3;
      v1 = t_data[i];
      if ( v1 >= lenv[0] ) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range \n", v1, i/3);
	  return 0;
	}
      v2 = t_data[i+1];
      if ( v2 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range \n", v2, i/3);
	  return 0;
	}
      v3 = t_data[i+2];
      if ( v3 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range \n", v3, i/3);
	  return 0;
	}
      
      triangle_normal( &v_data[v1*3], &v_data[v2*3], &v_data[v3*3], 
		       &trinorm[i] );
    }
    /* compute the vertices normals */
      tric = (int *)malloc(lenv[0] * sizeof(int));
      /*printf("vrnorm at %p, tric at %p\n", vnorm, tric);*/
      if (!tric)
	{
	  fprintf(stderr, "Failed to allocate memory for the normals('tric') \n");
	  free(trinorm);
	  return 0;
	}
      for (i=0; i<lenv[0]; i++)
	{
	  tric[i] = 0;
	  vnorm[i*3] = 0.0;
	  vnorm[i*3+1] = 0.0;
	  vnorm[i*3+2] = 0.0;
	}
      for (i=0; i<lent[0]*3; i+=3)    /*loop over triangles*/
	{
	  for (k=0; k<3; k++)          /*loop over vertices*/
	    {
	      tric[t_data[i+k]]++;
	      vnorm[3*t_data[i+k]] += trinorm[i];
	      vnorm[3*t_data[i+k]+1] += trinorm[i+1];
	      vnorm[3*t_data[i+k]+2] += trinorm[i+2];
	    }
	}
      for (i=0; i<lenv[0]; i++)
	{
	  if (tric[i] != 0)
	    for (k=0; k<3; k++) vnorm[i*3 + k] /= tric[i];
	}
      /*printf("free tric at %p\n", tric);*/
      free(tric);
      free (trinorm);
      return 1;
}

/*******************************************
The face and vertex normals are computed.
********************************************/
int triangleNormalsBoth(double *v_data, int lenv[2], float *vnorm,
			 int *t_data, int lent[2], float *trinorm)
{
  int   i, j, k, *tric;
  for (i=0; i<3*lent[0]; i+=3) 
    {
      int v1,v2,v3;
      v1 = t_data[i];
      if ( v1 >= lenv[0] ) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range \n", v1, i/3);
	  return 0;
	}
      v2 = t_data[i+1];
      if ( v2 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range \n", v2, i/3);
	  return 0;
	}
      v3 = t_data[i+2];
      if ( v3 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range \n", v3, i/3);
	  return 0;
	}
      
      triangle_normal( &v_data[v1*3], &v_data[v2*3], &v_data[v3*3], 
		       &trinorm[i] );
    }
  /* compute the vertices normals */
  tric = (int *)malloc(lenv[0] * sizeof(int));
  /*printf("vrnorm at %p, tric at %p\n", vnorm, tric);*/
  /** if (!vnorm || !tric) **/
  if (!tric)
    {
      fprintf(stderr, "Failed to allocate memory for the normals \n");
      return 0;
    }
  for (i=0; i<lenv[0]; i++)
    {
      tric[i] = 0;
      vnorm[i*3] = 0.0;
      vnorm[i*3+1] = 0.0;
      vnorm[i*3+2] = 0.0;
    }
  for (i=0; i<lent[0]*3; i+=3)    /*loop over triangles*/
    {
      for (k=0; k<3; k++)          /*loop over vertices*/
	{
	  tric[t_data[i+k]]++;
	  vnorm[3*t_data[i+k]] += trinorm[i];
	  vnorm[3*t_data[i+k]+1] += trinorm[i+1];
	  vnorm[3*t_data[i+k]+2] += trinorm[i+2];
	}
    }
  for (i=0; i<lenv[0]; i++)
    {
      if (tric[i] != 0)
	   for (k=0; k<3; k++) vnorm[i*3 + k] /= tric[i];
    }
  /*printf("free tric at %p\n", tric);*/
  free(tric);
  return 1;
}




typedef vector <double> DoubleVec;
typedef vector<vector<double> > DoubleArray;
typedef vector <int> IntVector;
typedef vector <vector<int> > IntArray;

//Remove duplicated vertices from supplied verts array and remap  faces. 
void removeDuplicatedVertices(float verts[][3], int *lenv, 
			      int *faces, int* lenf,
			      DoubleArray *newverts, 
			      IntArray *newfaces,
			      float norms[][3],
			      DoubleArray *newnorms)
{ 
  char st1[256];
  map<string, int > newInds;   // maps vertices to unique indices
  map <int, int> oldToNew;     // maps old indices to new, unique ones
  int nvert = 0;
  int i, j, ci;
  vector<double> v;
  vector<double> n;
  v.resize(3);
  n.resize(3);
  //printf ("len verts : %d, len faces: %d\n", lenv[0], lenf[0]);
  if (lenv[0]==0 || lenf[0]==0)
    {
      newverts->clear();
      newfaces->clear();
      newnorms->clear();
      return;
    }
  if (!norms)
    {
      newnorms->clear();
    }
  for (i = 0; i< lenv[0]; i++)
    {
      sprintf(st1, "%f%f%f\0", verts[i][0], verts[i][1], verts[i][2]);
      string st2(st1);
      if (newInds.find(st2) == newInds.end() )
	{
	  newInds[st2] = nvert;
	  oldToNew[i] = nvert;
	  nvert = nvert+1;
	  for (j = 0; j<3; j++)
	    {
	      //v.push_back(verts[i][j]);
	      v[j] = verts[i][j];
	      if (norms)
		{
		  n[j] = norms[i][j];
		}
	    }
	  newverts->push_back(v);
	  if (norms)
	    {
	      newnorms->push_back(n);
	    }
	}
      else 
	{
	  oldToNew[i] = newInds[st2];
	}
    }
  
  /***
      map<string, int>::iterator pos;
      for(pos=newInds.begin(); pos != newInds.end(); ++pos)
      {
      cout << "key:" << pos->first << "\t" << "value:" << pos->second << endl;
      }
  **/
  vector<int> f;
  f.resize(lenf[1]);
  for(i = 0; i<lenf[0]; i++)
    {
      ci = i*lenf[1];
      for(j = 0; j<lenf[1]; j++){
	//f.push_back(oldToNew[faces[ci+j]]);
	f[j] = oldToNew[faces[ci+j]];
      }
      newfaces->push_back(f);
	  //f.clear();
    }
}


float computeRMSD(float Coords[][3], float refCoords[][3], int lenCoords)
{
    float SumOfSquaredDev, dx, dy, dz, RMSD;
    int i;
     
    SumOfSquaredDev = 0.0;
     
    for ( i = 0; i < lenCoords; i++ )
    {
        dx = Coords[i][0] - refCoords[i][0];
        dy = Coords[i][1] - refCoords[i][1];
        dz = Coords[i][2] - refCoords[i][2];
        SumOfSquaredDev += dx*dx + dy*dy + dz*dz;
    }
     
    RMSD = sqrt( SumOfSquaredDev / lenCoords );
 
    return RMSD;
}

%}

%include numarr.i
/** typemaps for triangleNormals....() **/
%typemap(in) (double *v_data, int lenv[2]) (PyArrayObject *inv,
					 int expected_dims[2], int intdims[2])
%{
  expected_dims[0] = 0;
  expected_dims[1] = 3;
  inv = contiguous_typed_array($input, PyArray_DOUBLE, 2, expected_dims);
  if (! inv) return NULL;
  $1 = (double *)inv->data;
  intdims[0] = inv->dimensions[0];
  intdims[1] = inv->dimensions[1];
  $2 = intdims;
%}
%typemap(freearg)(double *v_data, int lenv[2])
%{
  if (inv$argnum)
    Py_DECREF(inv$argnum);
%}


%typemap(in) (int *t_data, int lent[2], float *trinorm) (PyArrayObject *intr, 
						      int expected_dims[2], int intdims[2])
%{
  expected_dims[0] = 0;
  expected_dims[1] = 3;
  intr = contiguous_typed_array($input, PyArray_INT, 2, expected_dims);
  if (! intr) return NULL;
  $1 = (int *)intr->data;
  intdims[0] = intr->dimensions[0];
  intdims[1] = intr->dimensions[1];
  $2 = intdims;
  $3 = (float *)malloc(intr->dimensions[0] * 3 * sizeof(float));
    if (!$3)
      {
	PyErr_SetString(PyExc_RuntimeError,
			"Failed to allocate memory for the normals");
	return NULL;
      }
%}

%typemap(argout) (int *t_data, int lent[2], float *trinorm)(PyArrayObject *out)
%{
  if ($result == NULL)
    {
      free($3);
      PyErr_SetString(PyExc_RuntimeError,"Failed to compute normals\n");
      return NULL;
    }
  out = (PyArrayObject *)PyArray_FromDimsAndData(2, $2,
						 PyArray_FLOAT, (char *)$3);
  if (!out)
    {
      PyErr_SetString(PyExc_RuntimeError,
		      "Failed to allocate memory for normals");
      return NULL;
    }

  // so we'll free this memory when this
  // array will be garbage collected
  MGL_SET_OWNDATA(out); 


  $result = l_output_helper2($result, (PyObject *)out);
%}

%typemap(freearg)(int *t_data, int lent[2], float *trinorm)
%{
  if (intr$argnum)
    Py_DECREF(intr$argnum);
%}

%typemap(out) int
%{
  if(!$1)
      $result = NULL;
  else
    {
      Py_INCREF(Py_None); 
      $result = Py_None;
    }
%}
int triangleNormalsPerFace(double *v_data, int lenv[2],  
			    int *t_data, int lent[2],
			    float *trinorm);

%typemap(in) (double *v_data, int lenv[2], float *vnorm) (PyArrayObject * inv,
						       int expected_dims[2], int intdims[2])
%{
  expected_dims[0] = 0;
  expected_dims[1] = 3;
  inv = contiguous_typed_array($input, PyArray_DOUBLE, 2, expected_dims);
  if (! inv) return NULL;
  $1 = (double *)inv->data;
  intdims[0] = inv->dimensions[0];
  intdims[1] = inv->dimensions[1];
  $2 = intdims;
  $3 = (float *)malloc(inv->dimensions[0] * 3 * sizeof(float));
  if (!$3)
    {
      PyErr_SetString(PyExc_RuntimeError,
		      "Failed to allocate memory for the vertex normals");
      return NULL;
    }
%}

%typemap(argout) (double *v_data, int lenv[2], float *vnorm)(PyArrayObject *out)
%{
  if ($result == NULL)
    {
      free($3);
      PyErr_SetString(PyExc_RuntimeError,"Failed to compute normals\n");
      return NULL;
    }
  out = (PyArrayObject *)PyArray_FromDimsAndData(2, $2,
						 PyArray_FLOAT, (char *)$3);
  if (!out)
    {
      PyErr_SetString(PyExc_RuntimeError,
		      "Failed to allocate memory for normals");
      return NULL;
    }

  // so we'll free this memory when this
  // array will be garbage collected
  MGL_SET_OWNDATA(out); 


  $result = l_output_helper2($result, (PyObject *)out);
  
%}

%typemap(freearg)(double *v_data, int lenv[2], float *vnorm)
%{
  if (inv$argnum)
    Py_DECREF(inv$argnum);
%}


%typemap (in) (int *t_data, int lent[2])(PyArrayObject *intr, 
				      int expected_dims[2], int intdims[2])
%{
  expected_dims[0] = 0;
  expected_dims[1] = 3;
  intr = contiguous_typed_array($input, PyArray_INT, 2, expected_dims);
  if (! intr) return NULL;
  $1 = (int *)intr->data;
  intdims[0] = intr->dimensions[0];
  intdims[1] = intr->dimensions[1];
  $2 = intdims;
%}
%typemap(freearg)(int *t_data, int lent[2])
%{
  if (intr$argnum)
    Py_DECREF(intr$argnum);
%}

int triangleNormalsPerVertex(double *v_data, int lenv[2], float *vnorm,
			      int *t_data, int lent[2]);
int triangleNormalsBoth(double *v_data, int lenv[2], float *vnorm,
			 int *t_data, int lent[2], float *trinorm);

/* Typemaps for removeDuplicatedVertices(): */

/* to output newverts as  a list from Python */

%typemap(in, numinputs=0) DoubleArray *newverts (DoubleArray temp)
 {
   $1 = &temp;
 }

%typemap(argout)  DoubleArray *newverts 
{
  int sizei, sizej;
  double element;
  PyObject * res;
  PyObject * res1;
  if (!(*$1).empty())
    {
      sizei = (*$1).size();
      sizej = (*$1)[0].size();
      res = PyList_New(sizei);
      for (unsigned int i=0; i<sizei; i++)
	{ 
	  res1 =  PyList_New(sizej);
	  for (unsigned int j=0; j<sizej; j++)
	    {
	      element =  (double)(*$1)[i][j];
	      //printf ("d = %f\n", element);
	      PyList_SetItem(res1 ,j, PyFloat_FromDouble(element));
	    }
	  PyList_SetItem(res,i, res1);
	}
    }
  else
    res = PyList_New(0);
  $result = t_output_helper2($result, res);

}

/* to output newfaces as a Python list */
%typemap(in, numinputs=0) IntArray *newfaces (IntArray temp)
 {

   $1 = &temp;
 }

%typemap(argout)  IntArray *newfaces
{
  int sizei, sizej;
  int element;
  PyObject * res;
  PyObject * res1;
  if (!(*$1).empty())
    {
      sizei = (*$1).size();
      sizej = (*$1)[0].size();
      res = PyList_New(sizei);
      for (unsigned int i=0; i<sizei; i++)
	{ 
	  res1 =  PyList_New(sizej);
	  for (unsigned int j=0; j<sizej; j++)
	    {
	      element =  (int)(*$1)[i][j];
	      PyList_SetItem(res1 ,j, PyInt_FromLong(element));
	    }
	  PyList_SetItem(res,i, res1);
	}
    }
  else
    res = PyList_New(0);
  $result = t_output_helper2($result, res);
}

/* to output newnorms as  a list from Python */

%typemap(in, numinputs=0) DoubleArray *newnorms (DoubleArray temp)
 {
   $1 = &temp;
 }

%typemap(argout)  DoubleArray *newnorms 
{
  int sizei, sizej;
  double element;
  PyObject * res;
  PyObject * res1;
  if (!(*$1).empty())
    {
      sizei = (*$1).size();
      sizej = (*$1)[0].size();
      res = PyList_New(sizei);
      for (unsigned int i=0; i<sizei; i++)
	{ 
	  res1 =  PyList_New(sizej);
	  for (unsigned int j=0; j<sizej; j++)
	    {
	      element =  (double)(*$1)[i][j];
	      //printf ("d = %f\n", element);
	      PyList_SetItem(res1 ,j, PyFloat_FromDouble(element));
	    }
	  PyList_SetItem(res,i, res1);
	}
      $result = t_output_helper2($result, res);
    }
}

/* to input verts as a Numeric arrray or a Python list */

%typemap(in) ( float verts[1][3],  int* lenv)(PyArrayObject *array, 
                                              int expected_dims[2],
                                              int intdims[2])
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
    intdims[0] = ((PyArrayObject *)(array))->dimensions[0];
    intdims[1] = ((PyArrayObject *)(array))->dimensions[1];
    $2 = intdims;
  }
  else
  { 
    array = NULL;
    $1 = NULL;
    $2 = 0;
  }
%}

%typemap(freearg) ( float verts[1][3],  int* lenv ) %{
   if (array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}




/* to input faces as a Numeric arrray or a Python list */
%typemap(in) ( int *faces,  int* lenf)(PyArrayObject *array, int intdims[2]) 

%{
  if ($input != Py_None)
  {

    array = contiguous_typed_array($input, PyArray_INT, 2, NULL);
    if (! array) return NULL;
    $1 = (int *)array->data;
    intdims[0] = ((PyArrayObject *)(array))->dimensions[0];
    intdims[1] = ((PyArrayObject *)(array))->dimensions[1];
    $2 = intdims;
  }
  else
  {
    array = NULL;
    $1 = NULL;
    $2 = NULL;
  }
%}

%typemap(freearg) (int *faces,  int* lenf) %{
   if (array$argnum)
      Py_DECREF((PyObject *)array$argnum);
%}

/* to input norms as a Numeric arrray or a Python list */
%apply float ARRAY2D[ANY][ANY] {float norms[1][3]};

void removeDuplicatedVertices(float verts[1][3], int *lenv, 
			      int *faces, int *lenf,
			      DoubleArray *newverts, 
			      IntArray *newfaces,
			      float norms[1][3] = NULL,
			      DoubleArray *newnorms= NULL);


%apply float ARRAY2D[ANY][ANY]{float Coords[1][3]};
%typemap(in) ( float refCoords[][3], int lenCoords)(PyArrayObject *array=NULL, 
                                                        int expected_dims[2])
%{
  if ($input != Py_None)
  {
    expected_dims[0] = 0;
    expected_dims[1] = $1_dim1;
    array = contiguous_typed_array($input, PyArray_FLOAT, 2, expected_dims);
    if (! array) return NULL;
    $1 = (float (*)[$1_dim1])array->data;
    $2 = ((PyArrayObject *)(array))->dimensions[0];
  }
  else
  { 
    $1 = NULL;
    $2 = 0;
  }
%}

%typemap(freearg) (float refCoords[][3], int lenCoords) %{
   if (array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

 
float computeRMSD(float Coords[1][3], float refCoords[][3], int lenCoords);

/*
   Compute normal vector for a bunch of triangles specified as:
     an n*3 sequence of floats for the vertices coordinates and
     an m*3 array of integers for the triangle's topology
   The thirds and optional (string) argument specifies the computation mode.
   This function can work in three different modes (default: 'PER_VERTEX'):
       'PER_FACE': computes the vector normal to each triangle using
                   triangle_normal. The resulting normals are returned
		   in a m*3 array of floats.
       'PER_VERTEX': after the face normals have been computed they normals
                     for each vertex are obtained by summing up the faces
		     normals of each triangle this vertex belongs to. The 
		     resulting normals are returned in a n*3 array of floats.
       'BOTH': The face and vertex normals are computed and both are returned.

   uses: contiguous_typed_array, triangle_normal
   available from the interpreter as:
          glTriangleNormals( vertices, triangles, | mode )
*/
/** Insert the python code into the python module containing shadow classes **/
%insert("shadow") %{

def TriangleNormals(vertices, triangles, mode = "PER_FACE" ):
    import mglcompat.oldnumeric as Numeric
    import numpy
    if (type(vertices) == Numeric.arraytype) \
      or (type(vertices) == numpy.ndarray):
        vertices = vertices.astype('f')
    if type(triangles) == Numeric.arraytype \
      or (type(triangles) == numpy.ndarray):
        triangles = triangles.astype('i')

    if mode == "PER_FACE":
        return triangleNormalsPerFace(vertices, triangles)
    elif mode == "PER_VERTEX":
        return triangleNormalsPerVertex(vertices, triangles)  
    elif mode == "BOTH":
        return triangleNormalsBoth(vertices, triangles) 
%}
