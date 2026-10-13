%module blur
%feature ("kwargs");
%init %{
  import_array(); /* load the Numeric PyCObjects */
%}

%{

#ifdef _MSC_VER
#include <windows.h>
#define WinVerMajor() LOBYTE(LOWORD(GetVersion()))
#endif

#include "numpy/arrayobject.h"
static PyObject* l_output_helper2(PyObject* target, PyObject* o) {
    PyObject*   o2;
    PyObject*   o3;
    if (!target) 
      {                   
        target = o;
      } 
    else if (target == Py_None)
      {  
        Py_DECREF(Py_None);
        target = o;
      } 
    else 
      {                         
        if (!PyList_Check(target))
	  {
            o2 = target;
            target = PyList_New(0);
            PyList_Append(target, o2);
            Py_XDECREF(o2);
	  }
        PyList_Append(target,o);
        Py_XDECREF(o);
      }
    return target;
}

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
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>


#define ELECTRON_DENSITY 0

float evalDensityInverse(float radius, float epsilon, int kernelType, double blobbyness)
{
	if (kernelType == 0)
		return radius * (float)sqrt(1.0 + log(epsilon) / blobbyness);
	else if (kernelType == 1)
		return (float)sqrt(radius*radius+log(epsilon)/blobbyness);
	else // this should crash
	{
		assert(kernelType != kernelType);
		return 0.0;
	}
}

void getBoundingBox(float coords[][3], float radii[], int na, 
		    float min[3], float max[3], double blobbyness, float padding)
  {
    int i, j;
    float maxRad=0.0;
    // use a fixed epsilon to avoid the growing bounding box problem
    // (1/10,000 should be sufficient)
    float epsilon =  1.0e-4;
    if(na == 0) 
      { // no atoms
	for(i = 0; i < 3; i++)
	  {
	    min[i] = max[i] = 0.0;
	  }
	return;
      }
    
    for(i = 0; i < 3; i++) 
      {
	min[i] = coords[0][i];
	max[i] = coords[0][i];
      }

	maxRad = evalDensityInverse(radii[0], epsilon, 1, blobbyness);
	//printf ("maxRad in getBoundingBox: %.2f\n", maxRad);
	
	for(j = 1; j < na; j++) 
	  {
	    for(i = 0; i < 3; i++)
	      {
		if(coords[j][i] < min[i]) min[i] = coords[j][i];
		if(coords[j][i] > max[i]) max[i] = coords[j][i]; 
	      }
	    
	    float tempRad = evalDensityInverse(radii[j], epsilon, 1, blobbyness);
	    //if (j<20)
	    //  printf("j = %d, tempRad = %.2f\n", j, tempRad);
	    if (maxRad < tempRad)
	      maxRad = tempRad;
	  }
	
	// adjust the bounding box by 1
	for(i = 0; i < 3; i++) 
	  {
	    min[i] -= maxRad;
	    max[i] += maxRad;
	    if (padding > 0)
	      {
		min[i] -= padding;
		max[i] += padding;
	      }
	  }
  }

float evalDensity(float position[3], float radius, float pnt[3], double maxRadius,
		  int densityType, int hydrophobicity, double blobby)
  {
    // Gaussian approximation
    //return 1.0;
    //printf("[%.3f, %.3f, %.3f], r=%.2f, pnt:[%.3f, %.3f, %.3f], mr=%.2f, b=%.2f, dt=%d", position[0],position[1], position[2],radius, pnt[0], pnt[1],pnt[2], maxRadius, blobby, densityType);
    double r = (position[0]-pnt[0])*(position[0]-pnt[0]) +
      (position[1]-pnt[1])*(position[1]-pnt[1]) +
      (position[2]-pnt[2])*(position[2]-pnt[2]);

    double r0 = radius; r0 *= r0;
    //double expval = blobby*r/r0 - blobby;
    //double expval = blobby*r - blobby*r0;
    double expval = blobby*((r/r0) - 1);
    
    // truncated gaussian
    //if (sqrt(r) > maxRadius)
    //	return 0.0;
    //printf ("r0=%.2f, ex=%.2f\n", r0, expval);
    float res;
    
    if( densityType == ELECTRON_DENSITY )
      res = (float)(exp(expval));
    //return (float)(exp(expval));
    else // hydrophobicity
      res = (float)(hydrophobicity*exp(expval));
    //return (float)(hydrophobicity*exp(expval));
    //printf ("%.4f,", res);
    return res;
    
  }


 void generateBlurmap(float coords[][3], float radii[], int dlen,
		  float *dens, int dim[3], double blobbyness, float orig[3], float span[3],  float weights[]=NULL, float gridMove[3]=NULL,float padding=0.0)
  {
   	float min[3],max[3], epsilon;
	unsigned int gdim[3];

	gdim[0] = dim[0]; gdim[1] = dim[1]; gdim[2] = dim[2];
	//float *dens = new float[dimx*dimy*dimz];
	
	epsilon = 1.0e-3;
	// compute the bounding box
	min[0] = min[1] = min[2] = 0.;
	max[0] = max[1] = max[2] = 0.;
	// getBoundingBox(coords, radii, dlen, min, max, epsilon, blobbyness);
	getBoundingBox(coords, radii, dlen, min, max, blobbyness, padding);

	//printf("bounding box: (%f,%f,%f)->(%f,%f,%f)\n", min[0],min[1],min[2],
	//        max[0],max[1],max[2]);
	// blur
	//float  *maxDens=0; 
	//float orig[3],span[3];
	unsigned int zoff=0, i, c1;

	//printf("epsilon = %f\n", epsilon);

	orig[0] = min[0]; //vol->getMinX();
	orig[1] = min[1]; //vol->getMinY();
	orig[2] = min[2]; //vol->getMinZ();
	
	if (gridMove)		// apply grid move
	{
	    orig[0] += gridMove[0];
	    orig[1] += gridMove[1];
	    orig[2] += gridMove[2];
	}

	// and span
	span[0] = (max[0] - min[0]) / (float)(dim[0]-1); //vol->getSpanX();
	span[1] = (max[1] - min[1]) / (float)(dim[1]-1); //vol->getSpanY();
	span[2] = (max[2] - min[2]) / (float)(dim[2]-1); //vol->getSpanZ();
	//printf("origin: %.2f, %.2f, %.2f \n", orig[0], orig[1], orig[2]);
	//printf("span: %.2f, %.2f, %.2f \n", span[0], span[1], span[2]);
	// main blurring loop
	c1 = dim[0]*dim[1];
	for (i=0; i < dlen; i++)
	  {
	    //PDBParser::Atom *at = atoms[i];
	    int hydrophobe=0;
	    double c[3], maxRad, r0;
	    unsigned int amax[3],amin[3], j,k,l, c2;

	    
	    //if (densityType != ELECTRON_DENSITY)
	    //hydrophobe = lookUpHydrophobicity(*at);
	    
	    // calculate the maximum radius for this atom
	    //maxRad = evalDensityInverse(at->getRadius(), epsilon, 1);
	    maxRad = evalDensityInverse(radii[i], epsilon, 1, blobbyness);
	    
	    // compute the dataset coordinates of the atom's center
	    for (j=0; j < 3; j++)
	      {
		c[j] = (coords[i][j] - orig[j]) / span[j];
		c[j] = ((c[j]-floor(c[j])) >= 0.5) ? ceil(c[j]) : floor(c[j]);
	      }
	    
	    // then compute the bounding box of the atom (maxRad^3)
	    
	    for (j=0; j < 3; j++)
	      {
		int tmp;
		tmp = (int)(c[j] - (maxRad / span[j]) - 1);
		tmp = (tmp < 0) ? 0 : tmp;
		amin[j] = (unsigned int)tmp;
		
		//amax[j] = (int)(c[j] + (maxRad / span[j]) + 1);
		//amax[j] = (amax[j] > gdim[j]) ? gdim[j] : amax[j];
		tmp = (int)(c[j] + (maxRad / span[j]) + 1);
		tmp = (tmp > gdim[j]) ? gdim[j] : tmp;
		amax[j] = (unsigned int)tmp;
	      }
	    //if (i<40)
	    //  printf("bbox = (%d,%d,%d)\n", amax[0]-amin[0],amax[1]-amin[1],amax[2]-amin[2]);
		
	    //printf("blur atom %d: (%u,%u,%u)->(%u,%u,%u)\n",i,
	    //							amin[0],amin[1],amin[2],
	    //								amax[0],amax[1],amax[2]);
		
		// begin blurring kernel
	    
	    for(l = amin[2]; l < amax[2]; l++)
	      {
		for(k = amin[1]; k < amax[1]; k++)
		  {
		    c2 = k*dim[0];
		      for(j = amin[0]; j < amax[0]; j++)
		      {
			int n = j + c2 + (l-zoff)*c1;
			//int n = j + k*dim[0] + (l-zoff)*dim[0]*dim[1];
			float pnt[3], density;
			
			pnt[0] = orig[0] + j*span[0];
			pnt[1] = orig[1] + k*span[1];
			pnt[2] = orig[2] + l*span[2];
			
			// calculate the electron density / hydrophobicity
			density = evalDensity( coords[i], radii[i], pnt, maxRad, ELECTRON_DENSITY, hydrophobe, blobbyness);
			// scaling density by weights
			if (weights){
			  density *= weights[i];
			}
			// sum up on a grid point
			dens[n] += density;
		      }
		  }
	      }
		// end blurring kernel
		
		// status message
		/****
		if (((i+1) % 20) == 0 || (i+1) == dlen)
		  {
		    printf("%2.2f%% done (%012d)\r", 100.0*(i+1)/(float)dlen, i+1);
		    fflush(stdout);
		  }
		***/
	  }
	// end of status line
	
	//printf("\n"); fflush(stdout);
	//printf ("Done\n");
  }
%}

// Typemaps for generateBlurmap()

// typemap to input the list of dimensions and ignore float *dens argument
%typemap(in) (float *dens, int dim[3])
%{
  int i, datasize, temp[3];
  PyObject *o;	
  
  if (!PyList_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expecting a list");
      return NULL;
    }
  if (PySequence_Length($input) != 3) 
    {
      PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected 3 elements");
      return NULL;
    }
  for (i = 0; i < 3; i++)
    {
      o = PyList_GetItem($input,i);
      if (PyNumber_Check(o)) 
	temp[i] = (int) PyInt_AsLong(o);
      else 
	{
	  PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
	  return NULL;
	}
    }
  datasize= temp[0]*temp[1]*temp[2];
  //allocate memory for data array - 'float *dens'
  //$1 = (float *) malloc(datasize*sizeof(float *));
   $1 = (float *) calloc( datasize, sizeof(float *));
   $2 = temp;
%}

//typemap to output the data array - dens
%typemap(argout ) float *dens
%{
  PyArrayObject * out;
  int dims[1];
  dims[0] = datasize;
  out = (PyArrayObject *)PyArray_FromDimsAndData(1, dims, PyArray_FLOAT, (char *)$1);
  //out = (PyArrayObject *)PyArray_FromDimsAndData(3, temp, PyArray_FLOAT, (char *)$1);
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

  //$result =  Py_BuildValue("O", (PyObject *)out) ;
    $result = l_output_helper2($result, (PyObject *)out);

%}

// typemap to input an array of radii
%typemap(in) (float radii[1], int dlen) (PyArrayObject *array, 
						      int expected_dims[1]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $1_dim0;
    if (expected_dims[0]==1) expected_dims[0]=0;
    array = contiguous_typed_array($input, PyArray_FLOAT, 1, expected_dims);
    if (! array) return NULL;
    $1 = (float *)array->data;
    $2 = ((PyArrayObject *)(array))->dimensions[0];
  }
  else
  {
    array = NULL;
    $1 = NULL;
    $2 = 0;
  }
%}

%typemap(freearg) (float radii[1], int dlen)
%{
   if (array$argnum)
      Py_DECREF((PyObject *)array$argnum);
%}

// typemap to input an array of weights (it should have the same length as radii)

%typemap(default) float weights[1]
%{
  
  PyArrayObject *weightsarr=NULL; 
  // have to make weightsarr local variable and use it in "freearg" typemap -
  // SWIG does not apply $argnum correctly in this case.
%}

%typemap(in) float weights[1] (int expected_dims[1]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0] = $1_dim0;
    if (expected_dims[0]==1) expected_dims[0]=0;
    weightsarr= contiguous_typed_array($input, PyArray_FLOAT, 1, expected_dims);
    if (! weightsarr) return NULL;
    $1 = (float *)weightsarr->data;
  }
  else
  {
    weightsarr = NULL;
    $1 = NULL;
  }

%}

%typemap(freearg) (float weights[1])
%{
  if (weightsarr)
     Py_DECREF((PyObject *)weightsarr);
%}

//typemap to input an array of coords
%typemap(in) float coords[1][3](PyArrayObject *array, 
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

  }
  else
  { 
    array = NULL;
    $1 = NULL;
  }
%}

%typemap(freearg) float coords[1][3]  
%{
   if (array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

// typemaps to ignore  'float orig[3]' and 'float span[3]' arguments

%typemap(in, numinputs=0) float OUT[3] 
%{
   $1 = (float *) malloc(sizeof(float)*3);
%}

// typemaps to output 'orig' and 'span' lists.
%typemap(argout) float OUT[3](float element, int i,
							  PyObject * res)
%{
  res = PyTuple_New(3);
  for (i=0; i<3; i++)
    { 
      element =  $1[i];
      PyTuple_SetItem(res ,i, PyFloat_FromDouble((double)element));
    }
  $result = l_output_helper2($result, res); 
%}

%typemap(freearg) float OUT[3] 
%{
   free($1);
%}

%typemap(default) float gridMove[3]
%{
    // have to make gridMoveVect a local variable and use it in "freearg" typemap -
  // SWIG does not apply $argnum correctly in this case.
  PyArrayObject *gridMoveVect=NULL;
 %}

%typemap(in) float gridMove[3](int expected_dims[1]) 
%{
  if ($input != Py_None)
  {
    expected_dims[0]=3;
    gridMoveVect = contiguous_typed_array($input, PyArray_FLOAT, 1, expected_dims);
    if (!gridMoveVect ) return NULL;
    $1 = (float *)gridMoveVect->data;
 }
else
  {
    gridMoveVect = NULL;
    $1 = NULL;
  }
%}

%typemap(freearg) float gridMove[3]

%{
  if (gridMoveVect)
      Py_DECREF((PyObject *)gridMoveVect);
   
%}

%apply float OUT[3] { float orig[3], float span[3], float min[3], float max[3]};


void generateBlurmap(float coords[1][3], float radii[1], int dlen,
		     float *dens, int dim[3], double blobbyness,
		     float orig[3], float span[3],  float weights[1]=NULL, float gridMove[3]=NULL, float padding=0.0);

void getBoundingBox(float coords[1][3], float radii[1], int dlen, 
		    float min[3], float max[3], double blobbyness, float padding=0.0);
/***
from Python interface the function is called as follows:
import blurr
#coords - array(n, 3) of xyz coodinates
# radii - array(n)  
data, origin, span = blur.generateBlurmap(coords, radii, [dimx, dimy,dimz],
                                          blobbyness)
***/
 
%include elementInfo.i

