%module bhtreelib
%{
#include "bhtree.h"
%}

//%include NumericArrays.i

%include numarr.i
FLOAT_ARRAY2D(xyz, [1][3], nbxyz)
FLOAT_VECTOR(rad, [1], nbrad)
INT_VECTOR( ids, [1], nb )

%typemap(out) int *
{
  if ($1)
    {
        PyArrayObject *array;
	int *ip, dims[2] = {1, 2};
	dims[0] = ($1[0]-1)/2;
	if (dims[0]==0)
	 {
	    $result = PyList_New(0);
	 }
	else 
	{
	     array =  (PyArrayObject *)PyArray_FromDims(2, dims, PyArray_INT);
	     memcpy( array->data, &$1[1], 2*dims[0]*sizeof(int) );
	     $result = (PyObject *)array;
	}
	free($1);
	  /*
	    , (char *)($1+1));
	      MGL_SET_OWNDATA(array);
	  */
	
    }
  else
    {
      $result = PyList_New(0);
    }
}

%typemap(out) int *closestPointsArray
{
  if ($1)
    {
        PyArrayObject *array;
	int *ip, dims[1];
	dims[0] = $1[0];
	if (dims[0]==0)
	 {
	    $result = PyList_New(0);
	 }
	else 
	{
	     array =  (PyArrayObject *)PyArray_FromDims(1, dims, PyArray_INT);
	     memcpy( array->data, &$1[1], dims[0]*sizeof(int) );
	     $result = (PyObject *)array;
	}
	free($1);
    }
  else
    {
      $result = PyList_New(0);
    }
}

%extend BHtree {

  /* Build a BHTree for a given set of 3D points and radii
	Granularity: average number of points in a leaf (10 is good)
   example:
        bht = bhtree.BHTree( pts, None, 10)
        result = Numeric.zeros( (400,) ).astype('i')
        nb = bht.ClosePoints(( 0.0, 0.0, 0.0 ), 10.0, result )
        dist = Numeric.zeros( (400,) ).astype('f')
  */
  BHtree ( float xyz[1][3], int *nbxyz, float rad[1], int nbrad, 
	   int granularity)
    {
      BHtree *self;
      BHpoint **BHat;
      int i, nb=nbxyz[0];
      float r, maxr;

      if (nbrad!=0 && nbxyz[0]!=nbrad) {
	PyErr_SetString(PyExc_ValueError, "Number of points and radii mismatch");
	return NULL;
      }

      BHat = (BHpoint **)malloc(nb*sizeof(BHpoint *));
      if (BHat == NULL) return NULL;

      maxr=0.0;
      for (i=0;i<nb;i++) {
	BHat[i] = (BHpoint *)malloc(sizeof(BHpoint));
	BHat[i]->x[0]=xyz[i][0];
	BHat[i]->x[1]=xyz[i][1];
	BHat[i]->x[2]=xyz[i][2];
	if (rad != NULL) r = rad[i];
	else r=0.0;
	BHat[i]->r=r;
	if (r > maxr) maxr = r;
	BHat[i]->at=i;
      }
      self = generateBHtree(BHat,nb,granularity);
      self->maxr = maxr;

      if (! self ) {
	PyErr_SetString(PyExc_RuntimeError, "Fail to build BHTree");
	return NULL;
      }
      return self;
    }

  ~BHTree() {
    freeBHtree(self);
  }

  /* Retrieve points in tree, withtin a given distance of a 3D point,
     the result is an array of point ids and a number of points

     NOTE: the result array must be allocated before the call, if it is
           too short (size=n), only the first n points are reported */ 
  int closePoints(float pt[3], float cutoff, int result[1], int maxn) {
    return findBHcloseAtoms(self, pt, cutoff, result, maxn);
  }

  /* Retrieve points in tree, withtin a given distance of a 3D point,
     the result is an array of point ids and a number of points along
     with an array of distances to the given 3D point

     NOTE: both result array must be allocated before the call, if it is
           too short (size=n), only the first n points are reported */ 
  int closePointsDist(float pt[3], float cutoff, int result[1], int maxn,float dist_2[1], int maxd) {
    return findBHcloseAtomsdist(self, pt, cutoff, result, dist_2, maxn);
  }

  /* Retrieve points in tree, within a given distance of a 3D point,
     the result is an array of point ids and a number of points along
     with an array of square of distances to the given 3D point

     NOTE: both result array must be allocated before the call, if it is
           too short (size=n), only the first n points are reported */ 
  int closePointsDist2(float pt[3], float cutoff, int result[1], int maxn,float dist_2[1], int maxd) {
    return findBHcloseAtomsdist2(self, pt, cutoff, result, dist_2, maxn);
  }

  /* Retrieve pairs of points in tree, such that their distance is less
     than tolerance*(sum of radii) */
  int *closePointsPairsInTree( float tolerance)
    {
      int *pairs = findClosePairsInTree(self, tolerance);
      pairs = findClosePairsInTree(self, tolerance);
      if (! self ) {
	PyErr_SetString(PyExc_RuntimeError, "Fail to build BHTree");
	return NULL;
      }
      return pairs;
    }

  /* Retrieve pairs of points (pi,pj) such that their distance is less
     than tolerance*(sum of radii) and pi is a point in the tree and pj
     is a point in the set provided to the function. */
  int *closePointsPairs( float xyz[1][3], int *nbxyz, float rad[1], 
			 int nbrad, float tolerance )
    {
      int *pairs;

      if (nbxyz[0]!=nbrad) {
	PyErr_SetString(PyExc_ValueError, "Number of points and radii mismatch");
	return NULL;
      }

      pairs = findClosePairs(self, xyz, nbxyz, rad, nbrad, tolerance);
      if (! self ) {
	PyErr_SetString(PyExc_RuntimeError, "Fail to build BHTree");
	return NULL;
      }
      return pairs;
    }

/* For every point in a given set of 3D points - xyz[1][3] -
   find a closest atom within a given cutoff. An array of
   corresponding atom indices is returned. */
 int *closestPointsArray (float xyz[1][3], int *nbxyz, float cut, int returnNullIfFail=1)
   {
    int *array;
    array = findClosestAtoms(self, xyz, nbxyz, cut, returnNullIfFail);
      if (! self ) {
	PyErr_SetString(PyExc_RuntimeError, "Fail to build BHTree");
	return NULL;
      }
      return array;
   }
}

FLOAT_ARRAY2D(points3D, [1][3], dims)

%{
/**************************************************************/
/*                Input: INT_VECTOR                           */
/**************************************************************/
static PyArrayObject *contiguous_typed_array1(PyObject *obj, int typecode,
			int expectnd, int *expectdims, int noRealloc)
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

  if (noRealloc==1) {
    if (arr!=obj) {
      sprintf(buf,"Numeric array of type %d was expected and not received\n",
	      typecode);
      PyErr_SetString(PyExc_ValueError, buf);
      return NULL;
    }
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

%define INOUT_INT_VECTOR( ARRAYNAME, ARRAYSHAPE, LENGTH )
%typemap(in) (int ARRAYNAME##ARRAYSHAPE, int LENGTH) (PyArrayObject *array, 
						      int expected_dims[1]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $1_dim0;
    if (expected_dims[0]==1) expected_dims[0]=0;
    array = contiguous_typed_array1($input, PyArray_INT, 1, expected_dims, 1);
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
     Py_DECREF((PyObject *)array$argnum);
%}
%enddef
INOUT_INT_VECTOR(result, [1], maxn)
FLOAT_VECTOR(dist_2,[1],maxd)

%include typemaps.i
%typemap(in) float[3](float temp[3]) {   /* temp[3] becomes a local variable*/
      int i;
      if (PyTuple_Check($input)) {
        if (!PyArg_ParseTuple($input,"fff",temp,temp+1,temp+2)) {
          PyErr_SetString(PyExc_TypeError,"tuple must have 3 elements");
          return NULL;
        }
        $1 = &temp[0];
      } else {
        PyErr_SetString(PyExc_TypeError,"expected a tuple.");
        return NULL;
      }
    }

%extend TBHTree {

  /* Build a TBHTree for a given set of 3D points
	ids: array of integers used to identify each point in the tree
	Granularity: average number of points in a leaf (10 is good)
	leafPadding: added space to allow for points motion in the tree (10)
	SpacePadding: margins added to the bounding box to insure that points
		      remain in bounding box after motion (99999.9)
   example:
        bht = bhtree.TBHTree( pts, ids, 10, 10, 9999.0 )
        result = Numeric.zeros( (400,) ).astype('i')
        nb = bht.ClosePoints(( 0.0, 0.0, 0.0 ), 10.0, result )
        dist = Numeric.zeros( (400,) ).astype('f')
        nb = bht.ClosePointsDist2(( 0.0, 0.0, 0.0 ), 10.0, result, dist )
        print Numeric.sqrt(dist[:nb])
  */
  TBHTree ( float points3D[1][3], int *dims, int ids[1], int nb,
	    int Granularity, int LeafPadding, float SpacePadding)
    {
      TBHTree *self;
      TBHPoint *Pts;
      float *fp = &points3D[0][0];
      int i, *ip = ids;

      if (dims[0] != nb) {
	PyErr_SetString(PyExc_RuntimeError, 
			"Array size mismatch between point and ids");
	return NULL;
      }
      Pts = (TBHPoint *)malloc(nb*sizeof(TBHPoint));
      if (!Pts) {
	PyErr_SetString(PyExc_RuntimeError,
			"Failed to allocate memory for points");
	return NULL;
      }
      for (i=0;i<nb;i++) {
	Pts[i].Pos[0]=*fp; fp++;
	Pts[i].Pos[1]=*fp; fp++;
	Pts[i].Pos[2]=*fp; fp++;
	Pts[i].ID= *ip; ip++;
      }

      self = GenerateTBHTree(Pts, nb, Granularity, LeafPadding, SpacePadding);
      if (! self ) {
	PyErr_SetString(PyExc_RuntimeError, "Fail to build TBHTree");
	return NULL;
      }
      return self;
    }

  ~TBHTree() {
    FreeTBHTree(self);
  }

  /* Retrieve points in tree, withtin a given distance of a 3D point,
     the result is an array of point ids and a number of points

     NOTE: the result array must be allocated before the call, if it is
           too short (size=n), only the first n points are reported */ 
  int ClosePoints(float pt[3], float cutoff, int result[1], int maxn) {
    return FindTBHCloseAtoms(self, pt, cutoff, result, maxn);
  }

  /* Retrieve points in tree, withtin a given distance of a 3D point,
     the result is an array of point ids and a number of points along
     with an array of square of distances to the given 3D point

     NOTE: both result array must be allocated before the call, if it is
           too short (size=n), only the first n points are reported */ 
  int ClosePointsDist2(float pt[3], float cutoff, int result[1], int maxn,float dist_2[1], int maxd) {
    return FindTBHCloseAtomsDist(self, pt, cutoff, result, dist_2, maxn);
  }

}

%extend RBHTree {

  /* Build a RBHTree for a given set of 3D points
	ids: array of integers used to identify each point in the tree
	Granularity: average number of points in a leaf (10 is good)
	leafPadding: added space to allow for points motion in the tree (10)
	InsertPadding:
	   Number of points that can be added to the tree. If limit is
          reached error message and error code returned.
        DeletePadding:
           Number of points that can be deleted from the tree.
	SpacePadding: margins added to the bounding box to insure that points
		      remain in bounding box after motion (99999.9)
   example:
        bht = bhtree.RBHTree( pts, ids, 10, 10, 10, 10, 9999.0 )
        result = Numeric.zeros( (400,) ).astype('i')
        nb = bht.ClosePoints(( 0.0, 0.0, 0.0 ), 10.0, result )
        dist = Numeric.zeros( (400,) ).astype('f')
        nb = bht.ClosePointsDist2(( 0.0, 0.0, 0.0 ), 10.0, result, dist )
        print Numeric.sqrt(dist[:nb])
  */
  RBHTree ( float points3D[1][3], int *dims, int ids[1], int nb,
	     int Granularity, int LeafPadding, int InsertPadding,
	     int DeletePadding, float SpacePadding, int OwnsMemory )
    {
      RBHTree *self;
      TBHPoint *Pts;
      float *fp = &points3D[0][0];
      int i, *ip = ids;

      if (dims[0] != nb) {
	PyErr_SetString(PyExc_RuntimeError, 
			"Array size mismatch between point and ids");
	return NULL;
      }
      Pts = (TBHPoint *)malloc(nb*sizeof(TBHPoint));
      if (!Pts) {
	PyErr_SetString(PyExc_RuntimeError,
			"Failed to allocate memory for points");
	return NULL;
      }
      for (i=0;i<nb;i++) {
	Pts[i].Pos[0]=*fp; fp++;
	Pts[i].Pos[1]=*fp; fp++;
	Pts[i].Pos[2]=*fp; fp++;
	Pts[i].ID= *ip; ip++;
      }

      self = GenerateRBHTree(Pts, nb, Granularity, LeafPadding, 
			     InsertPadding, DeletePadding, SpacePadding,
			     OwnsMemory);
      if (! self ) {
	PyErr_SetString(PyExc_RuntimeError, "Fail to build RBHTree");
	return NULL;
      }
      return self;
    }

  ~RBHTree() {
    FreeRBHTree(self);
  }

  /* Retrieve points in tree, withtin a given distance of a 3D point,
     the result is an array of point ids and a number of points

     NOTE: the result array must be allocated before the call, if it is
           too short (size=n), only the first n points are reported */ 
  int ClosePoints(float pt[3], float cutoff, int result[1], int maxn) {
    return FindRBHCloseAtoms(self, pt, cutoff, result, maxn);
  }

  /* Retrieve points in tree, withtin a given distance of a 3D point,
     the result is an array of point ids and a number of points along
     with an array of square of distances to the given 3D point

     NOTE: both result array must be allocated before the call, if it is
           too short (size=n), only the first n points are reported */ 
  int ClosePointsDist2(float pt[3], float cutoff, int result[1], int maxn,float dist_2[1], int maxd) {
    return FindRBHCloseAtomsDist(self, pt, cutoff, result, dist_2, maxn);
  }

  int InsertRBHPoint(float *Pos, float Rad, void *Data, int uInt, int *ID) {
    return InsertRBHPoint(self, Pos, Rad, Data, uInt, ID);
  }
  int DeleteRBHPoint(int ID) {
    return DeleteRBHPoint(self, ID);
  }
  int MoveRBHPoint(int ID, float NewPos[3], int FindDirection) {
    return MoveRBHPoint(self, ID, NewPos, FindDirection);
  }
}

/**
Return indices of faces (from a given array of faces fs[]) 
that contain nbVert number of vertices from a given subset of vertex indices (vs[])
**/
%{
int *findFaceSubset(int vs[], int vs_len, int fs[], int fs_len[2], 
		    int *count, int nbVert){
  int i, j, k, nx, ny, n, vsum, f, *new_fs;
  nx = fs_len[0];
  ny = fs_len[1];
  /*printf ("nx=%d, ny=%d \n", nx, ny);*/
  new_fs = (int *)malloc(nx*sizeof(int));
  if(!new_fs){
    printf ("failed to allocate memory for new_fs.\n");
    return NULL;
   }
  *count = 0;
  for (i=0; i<nx; i++){
    vsum = 0;
    n = ny*i;
    for (j=0; j<ny; j++){
      f = fs[n+j];
      /**printf ("i=%d, j=%d, f=%d\n", i,j,f);**/
      if (f!=-1){
	for (k=0; k<vs_len; k++){
	  if (f==vs[k]){
	    vsum ++;
	    break;
	  }
	}
      }
    }
    if (vsum>=nbVert){
      new_fs[*count] = i;
      (*count)++;
    }
  }
  /*printf("count=%d\n", *count); */
  if (*count < nx)
    new_fs = realloc(new_fs, (*count)*sizeof(int));
  return new_fs;
}
%}
INT_VECTOR( vs, [1], vs_len )
%typemap(in,numinputs=0) int *count
%{
  int arr_len;
  $1 = &arr_len;
%}

%typemap(in) (int fs[],  int fs_len[2])(PyArrayObject *array, int intdims[2])
%{
  if ($input != Py_None)
  {
    array = contiguous_typed_array($input, PyArray_INT, 2, NULL);
    if (! array) return NULL;
	  intdims[0] = ((PyArrayObject *)(array))->dimensions[0];
	  intdims[1] = ((PyArrayObject *)(array))->dimensions[1];
	  $2 = intdims;
    $1 = (int *)array->data;
  }
  else
  {
    array = NULL;
    $1 = NULL;
    $2 = 0;
  }
%}

%typemap(freearg) ( int fs[],  int* fs_len)
%{
   if (array$argnum)
      Py_DECREF((PyObject *)array$argnum);
%}

%typemap(out) int *findFaceSubset
{
  if ($1)
    {
        PyArrayObject *array;
	int *ip, dims[1];
        dims[0] = arr_len;
	if (dims[0]==0)
	 {
	    $result = PyList_New(0);
	 }
	else 
	{
	     array =  (PyArrayObject *)PyArray_FromDims(1, dims, PyArray_INT);
	     memcpy( array->data, $1, arr_len*sizeof(int) );
	     $result = (PyObject *)array;
	}
	free($1);
	
    }
  else
    {
      $result = PyList_New(0);
    }
}
int *findFaceSubset(int vs[1], int vs_len, int fs[], int fs_len[2], 
		    int *count, int nbVert);
%include "src/bhtree.h"





