
%module molderivatives



%{
#include "SumOfGaussiansCurvature.h"
#include <iostream>


void getGaussianCurvature (int numberOfGaussians, double gaussianCenters[][4],
			   int numberOfGridDivisions, 
			   double maxFunctionError, double blobbiness,
			   int numberOfPoints, float* points, 
			   double* HandK, double* normals, double* k1Vector,
			   double* k2Vector)
{
  
  SumOfGaussiansCurvature sum(numberOfGaussians, (double *)gaussianCenters, 
			      numberOfGridDivisions, maxFunctionError,
			      blobbiness, numberOfPoints, points, 
			      HandK, normals, k1Vector, k2Vector);
  //std::cout << "number of Points: " << numberOfPoints << std::endl;
  //std::cout << "number of Gaussians: " << numberOfGaussians << std::endl;
  bool status = sum.initialize();
  if (status)
    sum.getCurvatures();
  else 
    std::cerr << "Could not initialize SumOfGaussiansCurvature." << std::endl;
    
}

%}

%include "numarr.i"
%define INPUT_ARRAYPTR(type, pytype)
%typemap(in) (int LEN, type ARRAYPTR[][ANY])(PyArrayObject *array=NULL, int expected_dims[2])
%{
  expected_dims[0] = 0;
  expected_dims[1] =  $2_dim1;
  array = contiguous_typed_array($input, pytype,2 , NULL);
  if (! array) return NULL;
  $2 = (type (*)[$2_dim1])array->data;
  $1 = ((PyArrayObject *)(array))->dimensions[0];
%}

%typemap(freearg)(int LEN, type ARRAYPTR[][ANY])
%{
if (array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}
%enddef
   //INPUT_ARRAYPTR(float, PyArray_FLOAT);
INPUT_ARRAYPTR(double, PyArray_DOUBLE);


%apply  (int LEN, double ARRAYPTR[][ANY]) {(int numberOfGaussians, double gaussianCenters[][4])};

%typemap(arginit) (int numberOfPoints, float* points, 
	      double* HandK, double* normals, double* k1Vector,
	      double* k2Vector)
%{
  PyArrayObject *pts = NULL;
  PyArrayObject *handk, *norms, *vec1, *vec2;
%}
     
 
%typemap(in) (int numberOfPoints, float* points, 
	      double* HandK, double* normals, double* k1Vector,
	      double* k2Vector)
     

%{
  //PyArrayObject *pts = NULL;
  //PyArrayObject *handk, *norms, *vec1, *vec2;
  pts = contiguous_typed_array($input, PyArray_FLOAT, 2 , NULL);
  if (! pts) 
    return NULL;
  
  $1 = ((PyArrayObject *)(pts))->dimensions[0];

  $2 = (float *)pts->data;
  int hdims[1], ndims[1];
  hdims[0] = $1*2;
  ndims[0] = $1*3;
  
  handk = (PyArrayObject *)PyArray_FromDims(1, hdims, PyArray_DOUBLE);
  if (!handk)
    {
      std::cerr << "Failed to create PyArrayObject * handk (HandK)" <<std::endl;
      return NULL;
    }
  norms = (PyArrayObject *)PyArray_FromDims(1, ndims, PyArray_DOUBLE);
  if (!norms)
    {
      std::cerr << "Failed to create PyArrayObject *norms (normals)" <<std::endl;
      return NULL;
    }
  
  vec1 = (PyArrayObject *)PyArray_FromDims(1, ndims, PyArray_DOUBLE);
  if (!vec1)
    {
      std::cerr << "Failed to create PyArrayObject *vec1 (k1Vector)" <<std::endl;
      return NULL;
    }
  
  vec2 = (PyArrayObject *)PyArray_FromDims(1, ndims, PyArray_DOUBLE);
  if (!vec2)
    {
      std::cerr << "Failed to create PyArrayObject *vec2 (k2Vector)" <<std::endl;
      return NULL;
    }
  
  $3 = (double *)handk->data;
  $4 = (double *)norms->data;
  $5 = (double *)vec1->data;
  $6 = (double *)vec2->data;
%}

%typemap(argout) (double* HandK, double* normals, double* k1Vector,
		  double* k2Vector)
%{
  $result = Py_BuildValue("(NNNN)", handk, norms, vec1, vec2);
%}

%typemap(freearg)(int numberOfPoints, float* points, 
		  double* HandK, double* normals, double* k1Vector,
		  double* k2Vector) 
%{
  if (pts)
      Py_DECREF((PyObject *)pts);
  
%}

 
void getGaussianCurvature (int numberOfGaussians, double gaussianCenters[][4],
			   int numberOfGridDivisions, 
			   double maxFunctionError, double blobbiness,
			   int numberOfPoints, float* points, 
			   double* HandK, double* normals, double* k1Vector,
			   double* k2Vector);
