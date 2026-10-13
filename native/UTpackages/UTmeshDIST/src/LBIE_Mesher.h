#include <iostream>
#include <vector>
#include <math.h>

#include "octree.h"
#include "geoframe.h"
#include "mydrawer.h"

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

using namespace std;

//typedef vector<vector<float> > FloatArray;
//typedef vector <vector<int> > IntArray;
class LBIE_Mesher{

  public : 
        LBIE_Mesher(const char*);
        LBIE_Mesher(); 
        LBIE_Mesher(const char*, const char* , 
		    float , float, float, 
		    float , int );
	~LBIE_Mesher();
	Octree oc;
	MyDrawer drawer;
	void inputData(const unsigned char *data,  int dims[3], 
		       unsigned int numVerts, unsigned int numCells,
		       float origin[3] = NULL, float spans[3] = NULL);
	void fileOpen( const char* );
	void fileSave( const char* );
	void setMesh( int );
	
	void errorChange( float );
	void errorChange_in( float );
	void isovalueChange( float );
	void isovalueChange_in( float );

	void outTriangle(  float outverts[][3], int outfaces[][3]);
	void outTetra(float outverts[][3], int outfaces[][4]);
	void outHexa(float outverts[][3], int outfaces[][8]);
	void outQuad( float outverts[][3], int outfaces[][4]);
	int getNumFaces();
	int getNumVerts();
	float getVolMin();
	float getVolMax();
	void getOuterSurface(IntArray *newfaces);
	void setXCutPlane(float plane_x);
	void setZCutPlane(float plane_z);
	void getSurface(IntArray *newfaces, FloatArray * vertset, int crossection = 0);

    
  private :
        void saveTriangle( const char* );
        void saveTetra( const char* );
	void saveHexa( const char* );
	void saveQuad( const char* );

	float l_err;
	float l_err_in;
	float dist_level;

	int fopen_flag;
	char fname_buf[100];

	float isovalue;
	float isovalue_in;
	int dual_flag;
	int numFrames;
	double biggestDim;
	double centerx, centery, centerz;
	
	int meshtype;
	float iso_inner,iso_outer,inner_err_tol,outer_err_tol;

	geoframe* g_frames;
};


