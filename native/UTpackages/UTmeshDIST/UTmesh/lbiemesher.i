%feature ("kwargs");
%module lbiemesher
%{
#include "LBIE_Mesher.h"
%}
#include <iostream>
using namespace std;
%include "numarr.i"
%apply float ARRAY2D[ANY][ANY] { float outverts[1][3] }
%apply int ARRAY2D[ANY][ANY] { int outfaces[1][3] }
%apply int ARRAY2D[ANY][ANY] { int outfaces[1][4] }
%apply int ARRAY2D[ANY][ANY] { int outfaces[1][8] }
%apply int VECTOR[ANY] {int dims[3]}
%apply float VECTOR[ANY] {float origin[3]}
%apply float VECTOR[ANY] {float spans[3]}

#define DEFAULT_ERR		    0.0001f    //99.99f		//0.0001f
#define DEFAULT_ERR_IN      0.0001f 
#define DEFAULT_IVAL       -0.0001f    //-0.5000f	//-0.0001f  //-1.0331		0.0001
#define DEFAULT_IVAL_IN    -9.5001f    //10.000f

#define SINGLE	0
#define HEXA	1
#define	DOUBLE	2
#define	TETRA	3
#define	T_4_H	4
#define	TETRA2	5	

%typemap(python,in) (const unsigned char *data)(PyArrayObject *array)
{
	if ($input != Py_None)
	{
	  //array = contiguous_typed_array($input, PyArray_UBYTE,1, NULL);
	  array = contiguous_typed_array($input, PyArray_FLOAT,1, NULL);
    		if (! array) return NULL;
		
		$1 = (unsigned char *) array->data;
		//printf ("number of dimensions: %d \n", array->dimensions[0]);

	 }
  	 else
  	{
		array = NULL;

	}
}

typedef vector <vector<int> > IntArray;
typedef vector<vector<float> > FloatArray;

// to output newfaces as a Python list
//%include "fragments.i"
%include "typemaps.i"
%typemap(in, numinputs=0) IntArray *newfaces (IntArray temp)
 {

   $1 = &temp;
 }
/****
%typemap(argout )  IntArray *newfaces
{
  int sizei, sizej;
  int element;
  PyObject * res1;
  sizei = (*$1).size();
  $result = PyList_New(sizei);
  cout << "size of output vector: " << sizei << endl;
  if ( sizei > 0)
    {
      sizej = (*$1)[0].size();
      for (unsigned int i=0; i<sizei; i++)
	{ 
	  res1 =  PyList_New(sizej);
	  for (unsigned int j=0; j<sizej; j++)
	    {
	      element =  (int)(*$1)[i][j];
	      PyList_SetItem(res1 ,j, PyInt_FromLong(element));
	    }
	  PyList_SetItem($result,i, res1);
	}
    }
    
}
****/

%typemap(argout, fragment="t_output_helper")  IntArray *newfaces
{
  int sizei, sizej;
  int element;
  PyObject * res;
  PyObject * res1;
  sizei = (*$1).size();
  res = PyList_New(sizei);
  if ( sizei > 0)
    {
      sizej = (*$1)[0].size();
      
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
  $result = t_output_helper($result, res);
}

%typemap(in, numinputs=0) FloatArray *vertset (FloatArray temp)
 {
   $1 = &temp;
 }

%typemap(argout, fragment="t_output_helper")  FloatArray *vertset 
{
  int sizei, sizej;
  float element;
  PyObject * res;
  PyObject * res1;
  sizei = (*$1).size();
  res = PyList_New(sizei);
  if (sizei)
    {
      sizej = (*$1)[0].size();
      for (unsigned int i=0; i<sizei; i++)
	{ 
	  res1 =  PyList_New(sizej);
	  for (unsigned int j=0; j<sizej; j++)
	    {
	      element =  (float)(*$1)[i][j];
	      //printf ("d = %f\n", element);
	      PyList_SetItem(res1 ,j, PyFloat_FromDouble(element));
	    }
	  PyList_SetItem(res,i, res1);
	}
    }
  $result = t_output_helper($result, res);

}



	

class LBIE_Mesher
{

  public : 
        LBIE_Mesher(const char*);
        LBIE_Mesher();
        LBIE_Mesher(const char*, const char* , 
		    float , float, float, 
		    float , int );
	~LBIE_Mesher();
	Octree oc;
	void inputData(const unsigned char *data,
		       int dims[3], 
		       unsigned int numVerts, unsigned int numCells,
		       float origin[3] = NULL, float spans[3] = NULL);
	void fileOpen( const char* );
	void fileSave( const char* );
	void setMesh( int );
	
	void errorChange( float );
	void errorChange_in( float );
	void isovalueChange( float );
	void isovalueChange_in( float );

	void outTriangle(  float outverts[1][3], int outfaces[1][3]);
	void outTetra(float outverts[1][3], int outfaces[1][4]);
	void outHexa(float outverts[1][3], int outfaces[1][8]);
	void outQuad( float outverts[1][3], int outfaces[1][4]);
	int getNumFaces();
	int getNumVerts();
	float getVolMin();
	float getVolMax();
	void getOuterSurface(IntArray *newfaces);
	void setXCutPlane(float plane_x);
	void setZCutPlane(float plane_z);
	void getSurface(IntArray *newfaces, FloatArray * vertset, int crossection = 0);

};

/** %include "src/LBIE_Mesher.h" **/

