/*********************
This module wraps the fast isocontouring library developped at UT Austin.

The basic functionnality of this library is to create a 2D or 3D data set 
from which 2D or 3D iso-contoured can be extracted rapidly.
This version of the library supports multiple variables and timesteps on data 
sets of unsigned character, short and float.

  example:
      import isocontour, Numeric

      # create a data file from a file
      the_data = isocontour.loadDataset(isocontour.CONTOUR_FLOAT,
					isocontour.CONTOUR_REG_3D, 3, 1, 
	                                ["pot-2eti-glucose-3fields.raw"]);

      # compute an isocontour 
      isovar   = 0
      timestep = 0
      isovalue = 0.23
      isoc = isocontour.getContour3d(the_data, isovar, timestep, isovalue,
                                     isocontour.NO_COLOR_VARIABLE)

      # create numeric arrays for the geometry
      vert = Numeric.zeros((isoc.nvert,3)).astype('f')
      norm = Numeric.zeros((isoc.nvert,3)).astype('f')
      col = Numeric.zeros((isoc.nvert)).astype('f')
      tri = Numeric.zeros((isoc.ntri,3)).astype('i')

      # get the triangles
      isocontour.getContour3dData(isoc, vert, norm, col, tri)
**************************/
%module isocontour
%feature("python:cdefaultargs");
%{

#ifdef _MSC_VER
#include <windows.h>
#define WinVerMajor() LOBYTE(LOWORD(GetVersion()))
#endif

//#include <unistd.h>
#include <stdlib.h>
#include "contour.h"
#include "dataset.h"
#include "datareg3.h"
#include "datareg2.h"
#include "conplot2d.h"
%}

//%include typemaps.i
%include "numarr.i"
%include "Misc.i"          // typemaps for char **
%include "contour_i.hpp"

%extend DatasetInfo {
  void _dim( int OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->dim[0];
    OUT_VECTOR[1] = self->dim[1];
    OUT_VECTOR[2] = self->dim[2];
  }
  void _orig( float OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->orig[0];
    OUT_VECTOR[1] = self->orig[1];
    OUT_VECTOR[2] = self->orig[2];
  }
  void _span( float OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->span[0];
    OUT_VECTOR[1] = self->span[1];
    OUT_VECTOR[2] = self->span[2];
  }
  void _minext( float OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->minext[0];
    OUT_VECTOR[1] = self->minext[1];
    OUT_VECTOR[2] = self->minext[2];
  }
  void _maxext( float OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->maxext[0];
    OUT_VECTOR[1] = self->maxext[1];
    OUT_VECTOR[2] = self->maxext[2];
  }
}

%extend ConDataset {
  Signature *getSignature(int variable, int timestep, int funcnum) {
    int max = getNumberOfSignatures(self);
    if (!self->sfun[variable][timestep])
      getSignatureFunctions(self,variable, timestep);
    if (funcnum<0) funcnum=0;
    if (funcnum>max) funcnum=max-1;
    return &self->sfun[variable][timestep][funcnum];
    }
  }


%extend Signature {
  void getFx(float VECTOR[1]) {
    int i;
    for (i=0; i<self->nval; i++) VECTOR[i]=self->fx[i];
  }
  void getFy(float VECTOR[1]) {
    int i;
    for (i=0; i<self->nval; i++) VECTOR[i]=self->fy[i];
  }
}


// set the library's error handler
%{
void errorHandlerRaisePythonException(char *mystring, int x) 
{
  printf("ERROR caught by myhandler %s %d\n", mystring, x);
  PyErr_SetString(PyExc_NameError, mystring);
}
%}
%init %{
	setErrorHandler(errorHandlerRaisePythonException);
%}

%{
extern int verbose;
void delDatasetReg(ConDataset	*dataset)
{
  int i,t, v;			// timestep index variable
  Contour3d		*isocontour;
  if (dataset == NULL) return;
  if (verbose)
    printf ("delDatasetReg :\n");
  if (dataset->data)
    {
      if (dataset->sfun)
	{
	  for (v = 0; v < dataset->data->nData(); v++)	// delete signatures
	    {
	      if (verbose)
		printf ("delete dataset->sfun\n");
	      for (t = 0; t < dataset->data->nTime(); t++)
		if (dataset->sfun[v][t])
		  delete [] dataset->sfun[v][t];
	      delete [] dataset->sfun[v];
	    }
	  delete [] dataset->sfun;
	}
      if (verbose)
	printf ("delete dataset->data\n");
      delete dataset->data;
      dataset->data = NULL;
    }		// delete data, set to NULL
  if (dataset->plot)
    {
      if (verbose)
	printf ("delete dataset->plot\n");
      //isocontour = dataset->plot->getContour3d();
      //isocontour->~Contour3d();
      delete dataset->plot;
      dataset->plot = NULL;
    }
  if (dataset->vnames)
    {
      if (verbose)
	printf ("delete dataset->vnames\n");
      for (i = 0; i < dataset->data->nData(); i++)
	{ 
	  if (dataset->vnames[i])
	    {
	      delete [] dataset->vnames[i];
	    }
	}
      delete [] dataset->vnames;
    }
  delete dataset;
  /************
  if (dataset->data) delete dataset->data;
  if (dataset->plot) delete dataset->plot;
  if (dataset->sfun) delete dataset->sfun;
  if (dataset->vnames) free(dataset->vnames);
  *****************/
}


 void delContour3d (Contour3dData * contour)
  {
     delete contour;
   }


ConDataset *newDatasetRegFloat3D( int *dim, float data[1][1][1][1][1],
				 float orig[3], float span[3])
  {
    ConDataset * dataset = newDatasetReg( CONTOUR_FLOAT, CONTOUR_REG_3D, 
					  dim[1], dim[0],
			                  &dim[2], (u_char *)data); 
    ((Datareg3 *)dataset->data->getData(0))->setOrig(orig);
    ((Datareg3 *)dataset->data->getData(0))->setSpan(span); 
    return dataset;
  }
				 
ConDataset *newDatasetRegFloat2D( int *dim, float data[1][1][1][1],
				  float orig[2], float span[2])
  {
    ConDataset * dataset =  newDatasetReg( CONTOUR_FLOAT, CONTOUR_REG_2D,
                                           dim[1], dim[0],
			                   &dim[2], (u_char *)data);
    ((Datareg2 *)dataset->data->getData(0))->setOrig(orig); 
    ((Datareg2 *)dataset->data->getData(0))->setSpan(span); 
    return dataset;
  }
				 
ConDataset *newDatasetRegShort3D( int *dim, short data[1][1][1][1][1],
				  float orig[3], float span[3])
  {
    ConDataset * dataset = newDatasetReg( CONTOUR_USHORT, CONTOUR_REG_3D, 
                                          dim[1], dim[0],
			                  &dim[2], (u_char *)data);
    ((Datareg3 *)dataset->data->getData(0))->setOrig(orig);
    ((Datareg3 *)dataset->data->getData(0))->setSpan(span); 
    return dataset;
  }
				 
ConDataset *newDatasetRegShort2D( int *dim, short data[1][1][1][1],
				  float orig[2], float span[2])
  {
    ConDataset * dataset = newDatasetReg( CONTOUR_USHORT, CONTOUR_REG_2D,
                                          dim[1], dim[0],
			                  &dim[2], (u_char *)data);
    ((Datareg2 *)dataset->data->getData(0))->setOrig(orig); 
    ((Datareg2 *)dataset->data->getData(0))->setSpan(span); 
    return dataset;
  }
				 
ConDataset *newDatasetRegUchar3D( int *dim, u_char data[1][1][1][1][1],
				  float orig[3], float span[3])
  {
    ConDataset * dataset = newDatasetReg( CONTOUR_UCHAR, CONTOUR_REG_3D, 
                                          dim[1], dim[0],
			                  &dim[2], (u_char *)data);
    ((Datareg3 *)dataset->data->getData(0))->setOrig(orig);
    ((Datareg3 *)dataset->data->getData(0))->setSpan(span); 
    return dataset;
  }
				 
ConDataset *newDatasetRegUchar2D( int *dim, u_char data[1][1][1][1],
				  float orig[2], float span[2])
  {
    ConDataset * dataset = newDatasetReg( CONTOUR_UCHAR, CONTOUR_REG_2D, 
                                          dim[1], dim[0],
			                  &dim[2], (u_char *)data);
    ((Datareg2 *)dataset->data->getData(0))->setOrig(orig); 
    ((Datareg2 *)dataset->data->getData(0))->setSpan(span); 
    return dataset;
  }

void setOrig3D(ConDataset * dataset, float orig[3])
  {
	if (!dataset || !dataset->data || !dataset->plot)
	{
	errorHandler("setOrig3D: invalid dataset", FALSE);
	}
        ((Datareg3 *)dataset->data->getData(0))->setOrig(orig); 
  }
void setSpan3D(ConDataset * dataset, float span[3])
  {
	if (!dataset || !dataset->data || !dataset->plot)
	{
	errorHandler("setSpan3D: invalid dataset", FALSE);
	}
	    ((Datareg3 *)dataset->data->getData(0))->setSpan(span);
  }
void setOrig2D(ConDataset * dataset, float orig[2])
  {
	if (!dataset || !dataset->data || !dataset->plot)
	{
	errorHandler("setOrig2D: invalid dataset", FALSE);
	}
        ((Datareg2 *)dataset->data->getData(0))->setOrig(orig); 
  }
void setSpan2D(ConDataset * dataset, float span[2])
  {
	if (!dataset || !dataset->data || !dataset->plot)
	{
	errorHandler("setSpan2D: invalid dataset", FALSE);
	}
	    ((Datareg2 *)dataset->data->getData(0))->setSpan(span);
  }

%}

// "newDatasetRegFloat3D"
FLOAT_ARRAY5D(dim, data, [1][1][1][1][1])

%apply float VECTOR[ANY] { float orig[3] }
%apply float VECTOR[ANY] { float span[3] }
//
// Create a data set of 3D floats on a regular grid
//
// dim   : array of grid dimensions: [time][var][x][y][z]
// data  : nested sequences or 3D numeric array of float
// 
// example:
//  >>> # 10x10x10 cube, 1 variable, 1 time step
//  >>> a = Numeric.ones( (1,1,10,10,10) ).astype('f')
//  >>> d = newDatasetRegFloat3D( a )
//
//  >>> # 10x10x10 cube, 4variables, 1 time step
//  >>> a = Numeric.ones( (1,4,10,10,10) ).astype('f')
//  >>> d = newDatasetRegFloat3D( 4, 1, a )
//
//  NOTE: in the array the data is organized such that the first index varies 
//        the fastest and the last one the slowest 
extern ConDataset *newDatasetRegFloat3D(int *dim, float data[1][1][1][1][1],
				        float orig[3], float span[3]);

extern void delDatasetReg(ConDataset	*dataset);

extern void delContour3d (Contour3dData * contour);

//"newDatasetRegShort3D"
SHORT_ARRAY5D(dim, data, [1][1][1][1][1])
//
// Create a data set of 3D shorts on a regular grid
// see newDatasetRegFloat3D for explanation of arguments
extern ConDataset *newDatasetRegShort3D(int *dim, short data[1][1][1][1][1],
				        float orig[3], float span[3]);

//"newDatasetRegUchar3D"
UCHAR_ARRAY5D(dim, data, [1][1][1][1][1])
//
// Create a data set of 3D chars on a regular grid
// see newDatasetRegFloat3D for explanation of arguments
extern ConDataset *newDatasetRegUchar3D(int *dim, u_char data[1][1][1][1][1],
				        float orig[3], float span[3]);

extern void setOrig3D (ConDataset *dataset, float orig[3]);
extern void setSpan3D (ConDataset *dataset, float span[3]);

%apply float VECTOR[ANY] { float orig[2] } 
%apply float VECTOR[ANY] { float span[2] } 
//"newDatasetRegFloat2D"
FLOAT_ARRAY4D(dim, data, [1][1][1][1])
//
// Create a data set of 2D floats on a regular grid
// see newDatasetRegFloat3D for explanation of arguments
extern ConDataset *newDatasetRegFloat2D(int *dim, float data[1][1][1][1],
					float orig[2], float span[2]);
//"newDatasetRegShort2D"
SHORT_ARRAY4D(dim, data, [1][1][1][1])
//
// Create a data set of 2D short on a regular grid
// see newDatasetRegFloat3D for explanation of arguments
extern ConDataset *newDatasetRegShort2D(int *dim, short data[1][1][1][1],
					float orig[2], float span[2]);

// "newDatasetRegUchar2D"
UCHAR_ARRAY4D(dim, data, [1][1][1][1])
//
// Create a data set of 2D chars on a regular grid
// see newDatasetRegFloat3D for explanation of arguments
extern ConDataset *newDatasetRegUchar2D(int *dim, u_char data[1][1][1][1],
					float orig[2], float span[2]);
extern void setOrig2D (ConDataset *dataset, float orig[2]);
extern void setSpan2D (ConDataset *dataset, float span[2]);

%apply float ARRAY2D[ANY][ANY] { float vert[1][3] }
%apply float ARRAY2D[ANY][ANY] { float norm[1][3] }
%apply float VECTOR[ANY] { float vfun[1] }
%apply int ARRAY2D[ANY][ANY] { int tri[1][3] }
%{
void getContour3dData(Contour3dData *iscontour, float vert[1][3], 
		      float norm[1][3], float vfun[1], int tri[1][3],
	              int flipNormals)
  {
    int i, j;

    if (flipNormals) flipNormals=-1.0;
    else flipNormals=1.0;
    for (i=0; i < iscontour->nvert; i++) {
      for (j=0; j < 3; j++)
	{
	  vert[i][j] = iscontour->vert[i][j];
	  norm[i][j] = flipNormals*iscontour->vnorm[i][j];
	}
      vfun[i] = iscontour->vfun[i];
    }

    if (flipNormals==1.0) {
       for (i=0; i < iscontour->ntri; i++)
         {
	   tri[i][0] = iscontour->tri[i][0];
	   tri[i][1] = iscontour->tri[i][1];
	   tri[i][2] = iscontour->tri[i][2];
	 }
    } else {
       for (i=0; i < iscontour->ntri; i++)
         {
	   tri[i][0] = iscontour->tri[i][0];
	   tri[i][1] = iscontour->tri[i][2];
	   tri[i][2] = iscontour->tri[i][1];
	 }
    }
  }
%}
// "getContour3dData"
//
// Extract vertices, normals, vfun, and triangles from a 3D contour
// vert and norm are Numeric arrays of floats of shape [isontour->nvert][3]
// vfun is a Numeric arrays of floats of shape [isontour->nvert]
// tri is Numeric arrays of int of shape [isontour->ntri][3]
// flipNormals is an integer (1: flips normals, 0: doesn't)
extern void getContour3dData(Contour3dData *iscontour, float vert[1][3], 
			     float norm[1][3], float vfun[1], int tri[1][3],
			     int flipNormals);


%apply float ARRAY2D[ANY][ANY] { float vert[1][2] };
%apply int ARRAY2D[ANY][ANY] { int edge[1][2] };
%{
void getContour2dData(Contour2dData *iscontour, float vert[1][2],
		      int edge[1][2])
  {
    int i, j;

    for (i=0; i < iscontour->nvert; i++) {
      vert[i][0] = iscontour->vert[i][0];
      vert[i][1] = iscontour->vert[i][1];
    }
    
    for (i=0; i < iscontour->nedge; i++)
      {
	edge[i][0] = iscontour->edge[i][0];
	edge[i][1] = iscontour->edge[i][1];
      }
  }
%}
//"getContour2dData"
//
// Extract vertices and edges from a 2D contour
// vert is a Numeric arrays of floats of shape [isocontour->nvert][3]
// tri is Numeric arrays of int of shape [isontour->ntri][3]
extern void getContour2dData(Contour2dData *iscocontour, float vert[1][2],
			     int edge[1][2]);

//"Misc helpers"
%{
#include <stdlib.h>
static PyObject *string2Float(PyObject *self, PyObject *args) {
  char   *stringFloat;
  float  *floatlist;
  int    i;
  PyObject *list = NULL;
  PyArrayObject *array;
  
  if (!PyArg_ParseTuple(args, "O:string2Float", &list))
    return NULL;
  if (PyList_Check(list)) {
    int size = PyList_Size(list);
    int i = 0;
    floatlist = (float *) malloc((size)*sizeof(float));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem(list,i);
      if (PyString_Check(o))
	floatlist[i] = atof( PyString_AsString(o) );
      else {
        PyErr_SetString(PyExc_TypeError,"list must contain strings");
        free(floatlist);
        return NULL;
      }

      array = (PyArrayObject *)PyArray_FromDimsAndData(1, &size,
						       PyArray_FLOAT,
						       (char *)floatlist);

  // so we'll free this memory when this
  // array will be garbage collected
  MGL_SET_OWNDATA(array); 


    }
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
  return Py_BuildValue("O", array);
}
%}

%native(string2Float) string2Float;
%native (getSliceArray) PyObject *getSliceArray(PyObject *self, PyObject *args);
%{
static PyObject *getSliceArray(PyObject *self, PyObject *args) {
    PyObject * _resultobj;
    SliceData * _result;
    ConDataset * _arg0;
    PyArrayObject *slice_array;
    int  _arg1;
    int  _arg2;
    char  _arg3;
    int  _arg4;
    PyObject * _argc0 = 0;
    /**char _ptemp[128];**/
    int dims[2];

    if(!PyArg_ParseTuple(args,"Oiici:getSliceArray",&_argc0,&_arg1,&_arg2,&_arg3,&_arg4)) {
        return NULL;
       }
    if (_argc0) {
        //if (SWIG_GetPtr(_argc0,(void **) &_arg0,"_ConDataset_p")) {
	
        swig_type_info *ty = SWIG_TypeQuery("ConDataset *");
        if ((SWIG_ConvertPtr(_argc0,(void **) &_arg0, ty,1)) == -1){
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of getSlice. Expected p_ConDataset");
        return NULL;
        }
    }
    _result = (SliceData *)getSlice(_arg0,_arg1,_arg2,_arg3,_arg4);
    dims[0] = _result->width;
    dims[1] = _result->height;
    if (_result->datatype == CONTOUR_UCHAR) {
      slice_array = (PyArrayObject *)PyArray_FromDimsAndData(2, dims, \
				      PyArray_UBYTE, (char *)_result->ucdata);
    } else if (_result->datatype == CONTOUR_USHORT) {
      slice_array = (PyArrayObject *)PyArray_FromDimsAndData(2, dims, \
				      PyArray_SHORT, (char *)_result->usdata);
    } else { /*if (_result->datatype == CONTOUR_FLOAT) { */
      slice_array = (PyArrayObject *)PyArray_FromDimsAndData(2, dims, \
				      PyArray_FLOAT, (char *)_result->fdata);
    }

  // so we'll free this memory when this
  // array will be garbage collected
  MGL_SET_OWNDATA(slice_array); 


    return (PyObject *)slice_array;
}
%}
/**%native (getSliceArray) getSliceArray; **/
 
/**************** "References" ****************

C. Bajaj, V. Pascucci, D. Schikore. "Accelerated IsoContouring of Scalar 
Fields," chapter in: Data Visualization Techniques edited by C. Bajaj John 
Wiley and Sons

C. Bajaj, V.Pascucci, and D.Schikore. "The Contour Spectrum".
Proceedings of the 1997 IEEE Visualization Conference,167-173, October 1997
Phoeniz, Arizona.

C. Bajaj, V. Pascucci, D. Schikore. "Fast Isocontouring for Improved 
Interactivity". Proceedings: ACM Siggraph/IEEE Symposium on Volume 
Visualization, ACM Press, (1996), San Francisco, CA
***********************************************/

%pragma (python) include = "isocontour_include.py"

