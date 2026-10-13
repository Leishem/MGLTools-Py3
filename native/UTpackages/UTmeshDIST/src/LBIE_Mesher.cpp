#include <iostream>
#include <cstring>
#include <cstdlib>
#include <math.h>

#include "LBIE_Mesher.h"
#include "octree.h"
#include "geoframe.h"
#include "mydrawer.h"
using namespace std;

//constructor 1 : initializes a mesher for volumetric data (input - .rawiv file)
// sets iso values and error tolerance values, computes the mesh and
// writes vertices and faces into an output file (output - .raw)
LBIE_Mesher::LBIE_Mesher( const char* input, const char* output,
				  float in_iso_outer, float in_iso_inner, float in_error_outer,
				  float in_error_inner, int in_meshtype ){

	cout<< "input file is  "<<input<<"\n"
	    << "output file is "<<output<<"\n"
	    << "iso_outer is   "<<in_iso_outer<<"\n"
	    << "iso_inner is   "<<in_iso_inner<<"\n"
	    << "outer_err_tol is "<<in_error_outer<<"\n"
	    << "inner_err_tol is "<<in_error_inner<<"\n"
	    << "meshtype is    "<<in_meshtype<<"\n";

	// stuff carried over from original code
	fopen_flag = 0;
	dual_flag = 1;
	numFrames = 1;

	// initializing varables.
	iso_outer = in_iso_outer;
	iso_inner = in_iso_inner;

	outer_err_tol = in_error_outer;
	inner_err_tol = in_error_inner;

	//meshtype = in_meshtype;

	// reading in file
	fileOpen( input );

	// setting the mesh type
	setMesh( meshtype );

	// set the appropriate isovalues and error values
	if( iso_outer != DEFAULT_IVAL ) isovalueChange( iso_outer );
	if( outer_err_tol != DEFAULT_ERR ) errorChange( outer_err_tol );

	if( meshtype == DOUBLE || meshtype == TETRA2 ){
		if( iso_inner != DEFAULT_IVAL_IN ) isovalueChange_in( iso_inner );
		if( inner_err_tol != DEFAULT_ERR_IN ) errorChange_in( inner_err_tol );
	}

	// outputing to a file
	fileSave( output );
}

// constructor 2: initializes a Mesher for volumetric data stored in an
// input file (.rawiv)

LBIE_Mesher::LBIE_Mesher(const char* inputfile)
{
  // initializing varables.
  fopen_flag = 0;
  dual_flag = 1;
  numFrames = 1;

  iso_outer =  DEFAULT_IVAL; //outer surface
  iso_inner = DEFAULT_IVAL_IN; // inner surface

  outer_err_tol = DEFAULT_ERR;
  inner_err_tol = DEFAULT_ERR_IN;

  oc.setNormalType(0);    // added by A.O;  sets os.flag_normal
                          // FIXME : don't know what this
                          //flag is for - has to be initialized on Unix
  // reading in file
  fileOpen( inputfile );

}

// constructor 3: initializes default parameters of a Mesher
LBIE_Mesher::LBIE_Mesher()
{
  dual_flag = 1;
  numFrames = 1;

  iso_outer =  DEFAULT_IVAL; //outer surface
  iso_inner = DEFAULT_IVAL_IN; // inner surface

  outer_err_tol = DEFAULT_ERR;
  inner_err_tol = DEFAULT_ERR_IN;

  oc.setNormalType(0);    // added by A.O;  sets os.flag_normal
                          // FIXME : don't know what this
                          //flag is for - has to be initialized on Unix

  g_frames = new geoframe[numFrames];

  dist_level=20;

  l_err=DEFAULT_ERR;
  l_err_in=DEFAULT_ERR_IN;

  oc.set_isovalue(DEFAULT_IVAL);
  oc.set_isovalue_in(DEFAULT_IVAL_IN);
}

// input volumetric data
void LBIE_Mesher::inputData(const unsigned char *data,
		       int dims[3],
		       unsigned int numVerts,
		       unsigned int numCells,
		       float origin[3], float spans[3])
{
  oc.Octree_init_from_data(data, dims, numVerts, numCells, origin, spans);
  g_frames[0].calculateExtents();
}


LBIE_Mesher::~LBIE_Mesher(){

}
// set mesh type
void LBIE_Mesher::setMesh( int mymeshtype ){

	int n;
	meshtype = mymeshtype;
	dual_flag = 1;
    if ( mymeshtype == SINGLE ){
		n = 0;
	}
	else if ( mymeshtype == TETRA ){
		n = 1;
	}
	else if ( mymeshtype == HEXA ){
		n = 2;
	}
	else if ( mymeshtype == T_4_H ){
		n = 3;
	}
	else if ( mymeshtype == DOUBLE ){
		n = 4;
		dual_flag = 2;
	}
	else {        //should be tetra2
		n = 5;
		dual_flag = 2;
	}

	oc.setMeshType(n);

	if (dual_flag == 1) {			// single isovalue
		oc.collapse();
		oc.compute_qef();
		oc.traverse_qef(l_err);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}
	else if (dual_flag == 2) {		// interval volume
		oc.collapse_interval();
		oc.compute_qef_interval();
		oc.traverse_qef_interval(l_err, l_err_in);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}

}
//set error tolerance value for the outer surface
void LBIE_Mesher::errorChange( float err ){


	if (err <= 0) err = 0.0000001f;
	l_err = err;

	//cout<<"Inside errorChange. outer_err_tol is "<<err<<"\n";

	numFrames=1;

	delete [] g_frames;
	g_frames = 0;

	g_frames = new geoframe[numFrames];

	if (dual_flag == 1) {
		l_err = err;
		oc.traverse_qef(l_err);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}
	else if (dual_flag == 2) {
		l_err = err;
		oc.traverse_qef_interval(l_err, l_err_in);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}

}

//set error tolerance value for the inner surface
void LBIE_Mesher::errorChange_in( float err ){


	if (err <= 0) err = 0.0000001f;
	l_err=err;

	//cout<< "Inside errorChange_in. inner_err_tol is "<<err<<"\n";

	numFrames=1;

	delete [] g_frames;
	g_frames = 0;

	g_frames = new geoframe[numFrames];

	if (dual_flag == 1) {
		l_err = err;
		oc.traverse_qef(l_err);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}
	else if (dual_flag == 2) {
		l_err_in = err;
		oc.traverse_qef_interval(l_err, l_err_in);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}

}
//set isovalue for the outer surface
void LBIE_Mesher::isovalueChange( float myisovalue ){

  //cout<<"Inside isovalueChange. iso_outer is "<<myisovalue<<"\n";

  numFrames=1;
  delete [] g_frames;
  g_frames = 0;

  g_frames = new geoframe[numFrames];

  oc.set_isovalue(myisovalue);

  if (dual_flag == 1) {			// single isovalue
    oc.collapse();
    oc.compute_qef();
    oc.traverse_qef(l_err);
    oc.mesh_extract(g_frames[0], l_err);
    oc.quality_improve(g_frames[0]);
  }
  else if (dual_flag == 2) {		// interval volume
    oc.collapse_interval();
    oc.compute_qef_interval();
    oc.traverse_qef_interval(l_err, l_err_in);
    oc.mesh_extract(g_frames[0], l_err);
    oc.quality_improve(g_frames[0]);
  }
}

//set isovalue for the inner surface
void LBIE_Mesher::isovalueChange_in( float myisovalue_in ){

  //cout<<"Inside isovalueChange_in. iso_inner is "<<myisovalue_in<<"\n";

	numFrames=1;
	delete [] g_frames;
	g_frames = 0;
	g_frames = new geoframe[numFrames];

	oc.set_isovalue_in(myisovalue_in);
	dual_flag = 2;

	if (dual_flag == 1) {
		oc.collapse();
		oc.compute_qef();
		oc.traverse_qef(l_err);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}
	else if (dual_flag == 2) {
		oc.collapse_interval();
		oc.compute_qef_interval();
		oc.traverse_qef_interval(l_err, l_err_in);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}
}

// read volumetric data from a file
void LBIE_Mesher::fileOpen(const char* filename ){

	fopen_flag=1;

	//numFrames=1;
	//clearGeo(); Only do it once per frame.
	g_frames = new geoframe[numFrames];

	strcpy(fname_buf,filename);
	dist_level=20;

	l_err=DEFAULT_ERR;
	l_err_in=DEFAULT_ERR_IN;

	oc.set_isovalue(DEFAULT_IVAL);
	oc.set_isovalue_in(DEFAULT_IVAL_IN);
	oc.Octree_init(filename); // A.O
	/*** A.O
	if (dual_flag == 1) {			// single isovalue
		oc.Octree_init(filename);
		oc.collapse();
		oc.compute_qef();
		oc.traverse_qef(l_err);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}
	else if (dual_flag == 2) {		// interval volume
		oc.Octree_init(filename);
		oc.collapse_interval();
		oc.compute_qef_interval();
		oc.traverse_qef_interval(l_err, l_err_in);
		oc.mesh_extract(g_frames[0], l_err);
		oc.quality_improve(g_frames[0]);
	}

	***/
	g_frames[0].calculateExtents();
	/****
	biggestDim = g_frames[0].biggestDim;
	centerx = 0;
	centery = 0;
	centerz = 0;

	int c;
	for (c=0; c<numFrames; c++) {
		biggestDim = (biggestDim>g_frames[c].biggestDim? biggestDim : g_frames[c].biggestDim);
		centerx += g_frames[c].centerx;
		centery += g_frames[c].centery;
		centerz += g_frames[c].centerz;
	}

	centerx /= numFrames;
	centery /= numFrames;
	centerz /= numFrames;
	***/

}

// output mesh (vertices and faces) into a file

void LBIE_Mesher::fileSave( const char* filename ){

    if ( meshtype == SINGLE || meshtype == DOUBLE ) {
		saveTriangle( filename );
	}
	else if ( meshtype == TETRA || meshtype == TETRA2 ){
		saveTetra( filename );
	}
	else if ( meshtype == HEXA ){
		saveHexa( filename );
	}
	else if ( meshtype == T_4_H ){
		saveQuad( filename );
	}
}

// write triangle mesh into a file
void LBIE_Mesher::saveTriangle( const char* filename ){

	FILE* fp;

	fp=fopen(filename,"w");
	int v0, v1, v2, nv, ntri, i;

	float r, a, b, c, p, sum_area, center;
	sum_area = 0.0;
	center = 64.0f/2.0f;

	nv=g_frames->getNVert();
	ntri=g_frames->getNTri();

	fprintf(fp,"%d %d\n", nv, ntri);
	for (i=0;i<nv;i++) {
		r = (g_frames->verts[i][0]-center)*(g_frames->verts[i][0]-center) +
					(g_frames->verts[i][1]-center)*(g_frames->verts[i][1]-center) +
					(g_frames->verts[i][2]-center)*(g_frames->verts[i][2]-center);
		fprintf(fp,"%f %f %f\n",g_frames->verts[i][0],g_frames->verts[i][1],g_frames->verts[i][2]);
		//fprintf(fp,"%f %f %f\n", sqrt(r)*0.3125, sqrt(r)*0.46875-10.0, fabs(sqrt(r)*0.46875-10.0)/10.0);
	}
	for (i=0;i<g_frames->getNTri();i++) {
		v0 = g_frames->triangles[i][0];
		v1 = g_frames->triangles[i][1];
		v2 = g_frames->triangles[i][2];
		r = (g_frames->verts[v0][0]-center)*(g_frames->verts[v0][0]-center) +
					(g_frames->verts[v0][1]-center)*(g_frames->verts[v0][1]-center) +
					(g_frames->verts[v0][2]-center)*(g_frames->verts[v0][2]-center);
		if(sqrt(r) < 17.0) {
			a = sqrt((g_frames->verts[v1][0]-g_frames->verts[v0][0])*(g_frames->verts[v1][0]-g_frames->verts[v0][0]) +
					 (g_frames->verts[v1][1]-g_frames->verts[v0][1])*(g_frames->verts[v1][1]-g_frames->verts[v0][1]) +
					 (g_frames->verts[v1][2]-g_frames->verts[v0][2])*(g_frames->verts[v1][2]-g_frames->verts[v0][2]));
			b = sqrt((g_frames->verts[v2][0]-g_frames->verts[v1][0])*(g_frames->verts[v2][0]-g_frames->verts[v1][0]) +
					 (g_frames->verts[v2][1]-g_frames->verts[v1][1])*(g_frames->verts[v2][1]-g_frames->verts[v1][1]) +
					 (g_frames->verts[v2][2]-g_frames->verts[v1][2])*(g_frames->verts[v2][2]-g_frames->verts[v1][2]));
			c = sqrt((g_frames->verts[v0][0]-g_frames->verts[v2][0])*(g_frames->verts[v0][0]-g_frames->verts[v2][0]) +
				     (g_frames->verts[v0][1]-g_frames->verts[v2][1])*(g_frames->verts[v0][1]-g_frames->verts[v2][1]) +
					 (g_frames->verts[v0][2]-g_frames->verts[v2][2])*(g_frames->verts[v0][2]-g_frames->verts[v2][2]));
			p = (a + b + c)/2.0;
			sum_area += sqrt(p*(p - a)*(p - b)*(p - c));
		}
		fprintf(fp,"%d %d %d\n",g_frames->triangles[i][0],g_frames->triangles[i][1],g_frames->triangles[i][2]);
	}
	//fprintf(fp, "area_14.5 = %f %f\n", sum_area, 4.0*3.1415926*10*10);
	fclose(fp);
}

//write tetrahedral mesh into a file
void LBIE_Mesher::saveTetra( const char* filename ){

	FILE* fp;
	fp=fopen(filename,"w");
	int nv, ntri, i;
	float center = (129.0f-1.0f)/2.0f;

	nv=g_frames->getNVert();
	ntri=g_frames->getNTri();

	fprintf(fp,"%d %d\n",nv,ntri/4);
	for (i=0;i<nv;i++) {
		//fprintf(fp,"%f %f %f\n",g_frame->verts[i][0],g_frame->verts[i][1],g_frame->verts[i][2]);
		//float r =	(g_frame->verts[i][0]-center)*(g_frame->verts[i][0]-center)*0.6798984*0.6798984 +
		//			(g_frame->verts[i][1]-center)*(g_frame->verts[i][1]-center)*0.6798984*0.6798984 +
		//			(g_frame->verts[i][2]-center)*(g_frame->verts[i][2]-center)*0.6779922*0.6779922;
		//fprintf(fp,"%f %f %f\n",(g_frame->verts[i][0]-center)*0.6798984*2,(g_frame->verts[i][1]-center)*0.6798984*2,
		//	(g_frame->verts[i][2]-center)*0.6779922*2);
		fprintf(fp,"%f %f %f\n",(g_frames->verts[i][0]-center),(g_frames->verts[i][1]-center),
			(g_frames->verts[i][2]-center));
	}
	for (i=0;i<ntri/4;i++) {
		fprintf(fp,"%d %d %d %d\n",g_frames->triangles[4*i][0],g_frames->triangles[4*i][1],
			g_frames->triangles[4*i][2], g_frames->triangles[4*i+1][2]);
	}
	fclose(fp);
}

//write hexahedral mesh into a file
void LBIE_Mesher::saveHexa( const char* filename ){

	FILE* fp;
	fp = fopen(filename,"w");
	int nv, nquad, i;

	nv = g_frames->getNVert();
	nquad = g_frames->getNQuad();

	fprintf(fp,"%d %d\n", nv, nquad/6);
	for (i = 0; i < nv; i++) {
		//fprintf(fp,"%f %f %f %f %f %f %d\n", g_frames->verts[i][0], g_frames->verts[i][1], g_frames->verts[i][2],
		//	g_frames->normals[i][0], g_frames->normals[i][1], g_frames->normals[i][2], g_frames->bound_sign[i]);
		fprintf(fp,"%f %f %f %d\n", g_frames->verts[i][0], g_frames->verts[i][1], g_frames->verts[i][2], g_frames->bound_sign[i]);
	}
	for (i = 0;i < nquad/6; i++) {
		fprintf(fp,"%d %d %d %d %d %d %d %d\n", g_frames->quads[6*i][0], g_frames->quads[6*i][1],
							g_frames->quads[6*i][2], g_frames->quads[6*i][3], g_frames->quads[6*i+1][1],
							g_frames->quads[6*i+1][0], g_frames->quads[6*i+1][3], g_frames->quads[6*i+1][2]);
	}
	fclose(fp);

}

// write quads into a file
void LBIE_Mesher::saveQuad( const char* filename ){

	FILE* fp;
	fp = fopen(filename,"w");
	int nv, nquad, i;

	nv=g_frames->getNVert();
	nquad=g_frames->getNQuad();


	// Output 4-node quad element
	fprintf(fp,"%d %d\n", nv, nquad);
	for (i = 0; i < nv; i++) {
		fprintf(fp,"%f %f %f\n", g_frames->verts[i][0], g_frames->verts[i][1], g_frames->verts[i][2]);
	}
	for (i = 0; i < g_frames->getNQuad(); i++) {
		fprintf(fp,"%d %d %d %d\n", g_frames->quads[i][0], g_frames->quads[i][1], g_frames->quads[i][2], g_frames->quads[i][3]);
	}

	fclose(fp);
}

//output vertices an faces of mesh triangles
void LBIE_Mesher::outTriangle(  float outverts[][3], int outfaces[][3] )
{

	int nv, ntri, i;

	nv=g_frames->getNVert();
	ntri=g_frames->getNTri();

	for (i=0; i<nv; i++)
	  {
	    outverts[i][0] = g_frames->verts[i][0];
	    outverts[i][1] = g_frames->verts[i][1];
	    outverts[i][2] = g_frames->verts[i][2];
	  }
	for (i=0; i<ntri; i++)
	  {
	    //outfaces[i][0] = g_frames->triangles[i][0];
	    outfaces[i][0] = g_frames->triangles[i][2];
	    outfaces[i][1] = g_frames->triangles[i][1];
	    //outfaces[i][2] = g_frames->triangles[i][2];
	    outfaces[i][2] = g_frames->triangles[i][0];
	  }
}

// output tetrahedral mesh
void LBIE_Mesher::outTetra(float outverts[][3], int outfaces[][4])
{

	int nv, ntri, i;
	float center = (129.0f-1.0f)/2.0f;

	nv=g_frames->getNVert();
	ntri=g_frames->getNTri();

	for (i=0;i<nv;i++)
	  {
	    outverts[i][0] = g_frames->verts[i][0]-center;
	    outverts[i][1] = g_frames->verts[i][1]-center;
	    outverts[i][2] = g_frames->verts[i][2]-center;
	  }
	for (i=0;i<ntri/4;i++)
	  {
	    outfaces[i][0] = g_frames->triangles[4*i][0];
	    outfaces[i][1] = g_frames->triangles[4*i][1];
	    outfaces[i][2] = g_frames->triangles[4*i][2];
	    outfaces[i][3] = g_frames->triangles[4*i+1][2];
	  }
}

//output hexahedral mesh
void LBIE_Mesher::outHexa(float outverts[][3], int outfaces[][8])
{
  int nv, nquad, i;

  nv = g_frames->getNVert();
  nquad = g_frames->getNQuad();

  for (i = 0; i < nv; i++) {
    outverts[i][0] = g_frames->verts[i][0];
    outverts[i][1] = g_frames->verts[i][1];
    outverts[i][2] = g_frames->verts[i][2];
  }
  for (i = 0;i < nquad/6; i++) {
     outfaces[i][0]= g_frames->quads[6*i][0];
     outfaces[i][1]= g_frames->quads[6*i][1];
     outfaces[i][2]= g_frames->quads[6*i][2];
     outfaces[i][3]= g_frames->quads[6*i][3];
     outfaces[i][4]= g_frames->quads[6*i+1][1];
     outfaces[i][5]= g_frames->quads[6*i+1][0];
     outfaces[i][6]= g_frames->quads[6*i+1][3];
     outfaces[i][7]= g_frames->quads[6*i+1][2];

  }


}

//output vertices and faces of quadrilateral mesh
void LBIE_Mesher::outQuad( float outverts[][3], int outfaces[][4])
{
	int nv, nquad, i;

	nv=g_frames->getNVert();
	nquad=g_frames->getNQuad();


	// Output 4-node quad element
	for (i = 0; i < nv; i++)
	  {
	    outverts[i][0] =  g_frames->verts[i][0];
	    outverts[i][1] = g_frames->verts[i][1];
	    outverts[i][2] = g_frames->verts[i][2];
	  }
	for (i = 0; i < nquad; i++)
	  {
	    /**
	    outfaces[i][0] =  g_frames->quads[i][0];
	    outfaces[i][1] = g_frames->quads[i][1];
	    outfaces[i][2] = g_frames->quads[i][2];
	    outfaces[i][3] = g_frames->quads[i][3];
	    **/
	    outfaces[i][0] =  g_frames->quads[i][3];
	    outfaces[i][1] = g_frames->quads[i][2];
	    outfaces[i][2] = g_frames->quads[i][1];
	    outfaces[i][3] = g_frames->quads[i][0];
	}

}

int LBIE_Mesher::getNumFaces()
{
    if ( meshtype == SINGLE || meshtype == DOUBLE )
      return g_frames->getNTri();

    else if ( meshtype == TETRA || meshtype == TETRA2 )
      return g_frames->getNTri()/4;

    else if ( meshtype == HEXA )
      return g_frames->getNQuad()/6;

    else if ( meshtype == T_4_H )
      return g_frames->getNQuad();
}

int LBIE_Mesher::getNumVerts()
{
  return g_frames->getNVert();
}

//return maximum value of the volumetric data
float LBIE_Mesher::getVolMax()
{
  return oc.vol_max;
}

//return minimum value of the volumetric data
float LBIE_Mesher::getVolMin()
{
  return oc.vol_min;
}

//return outer faces of tetrahedral or hexahedral meshes
void LBIE_Mesher:: getOuterSurface(IntArray *newfaces)
{
  float norm[3];
  int face[3], vert, my_bool, my_bool_0, c, num_tri;
  num_tri = 0;
  //vector<float> v;
  vector<int> f;
  if (g_frames->numtris )
    {
      for (c = 0; c < g_frames->numtris; c++)
	{

	  my_bool =	(g_frames->bound_sign[g_frames->triangles[c][0]] == 1) &&
	    (g_frames->bound_sign[g_frames->triangles[c][1]] == 1) &&
	    (g_frames->bound_sign[g_frames->triangles[c][2]] == 1);

	  my_bool_0 = (g_frames->bound_sign[g_frames->triangles[c][0]] == -1) &&
	    (g_frames->bound_sign[g_frames->triangles[c][1]] == -1) &&
	    (g_frames->bound_sign[g_frames->triangles[c][2]] == -1);

	  if(my_bool || my_bool_0)
	    {
	      num_tri = num_tri +1;

	      for(int i = 0; i < 3; i++)
		{
		  vert = g_frames->triangles[c][i];
		  face[i] = vert;
		  norm[0] = g_frames->normals[vert][0];
		  norm[1] = g_frames->normals[vert][1];
		  norm[2] = g_frames->normals[vert][2];
		  //v.push_back(g_frames->verts[vert][0]);
		  //v.push_back(g_frames->verts[vert][1]);
		  //v.push_back(g_frames->verts[vert][2]);
		  //f.push_back(vert);
		}
	      f.push_back(face[2]);
	      f.push_back(face[1]);
	      f.push_back(face[0]);
	      newfaces->push_back(f);
	      f.clear();
	      //v.clear();
	    }
	}
    } //if (g_frames->numtris )
  else
    {
      for (c=0; c<g_frames->numquads; c++)
	{
	  my_bool =	abs((int)g_frames->bound_sign[g_frames->quads[c][0]]) == 1 &&
	    abs((int)g_frames->bound_sign[g_frames->quads[c][1]]) == 1 &&
	    abs((int)g_frames->bound_sign[g_frames->quads[c][2]]) == 1 &&
	    abs((int)g_frames->bound_sign[g_frames->quads[c][3]]) == 1;
	  if(my_bool)
	    {
	      f.push_back(g_frames->quads[c][3]);
	      f.push_back(g_frames->quads[c][2]);
	      f.push_back(g_frames->quads[c][1]);
	      f.push_back(g_frames->quads[c][0]);
	      newfaces->push_back(f);
	      f.clear();
	    }
	}
    } //else
}
void LBIE_Mesher::setXCutPlane(float planex)
{
  drawer.setPlaneX(planex);
}

void LBIE_Mesher::setZCutPlane(float planez)
{
  drawer.setPlaneZ(planez);
}

void LBIE_Mesher:: getSurface(IntArray *newfaces, FloatArray * vertset, int crossection )
{
  drawer.setCutting(crossection);
  drawer.setGeo(g_frames);
  drawer.display(newfaces, vertset);
}
