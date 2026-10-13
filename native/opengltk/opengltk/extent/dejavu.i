%feature ("kwargs");
/* for optional import in util.i */

/*******************************************************************
*******************************************************************
 C functions to support DejaVu, will be generalized later

*******************************************************************
*******************************************************************/
/*
 * copyright_notice
 */

%{


#ifdef _MSC_VER
#include <windows.h>
#define WinVerMajor() LOBYTE(LOWORD(GetVersion()))
#endif

  /** #define METH_VARARGS METH_KEYWORDS **/

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "mgl_npcompat.h"


#define TRY(E) if(! (E)) return NULL

/* MS March 4 '99 */

/********************************************************************
  Tries to create a contiguous numeric array of type typecode from a 
  Python object. Works for list, tuples and numeric arrays.

  obj: Numeric array Python object
  typecode: data type PyArray_{ CHAR, UBYTE, SBYTE, SHORT, INT, LONG, FLOAT,
                                DOUBLE, CFLOAT, CDOUBLE }
  expectnd: required number of dimensions. Used for checking. Ignored if <=0.
  expectdims: array of expected extends. Used for checking. Ignored if <=0.

  Raises ValueError exceptions if:
  - the PyArray_ContiguousFromObject fails
  - the array has a bad shape
  - the extent of a given dimension doesn't match the specified extent.
********************************************************************/

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

static PyObject* l_output_helper2(PyObject* target, PyObject* o) {
    PyObject*   o2;
    if (!target) {                   
        target = o;
    } else if (target == Py_None) {  
        Py_DECREF(Py_None);
        target = o;
    } else {                         
        if (!PyList_Check(target)) {
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

/*
	When composing successively rotations the resulting matrix often
	becomes nonorthogonal leading to skewing and scaling effects.
	This function takes a 4x4 matrix of DOUBLE that represents a OpenGL
	transformation and reorthogonalizes it
	uses: contiguous_typed_array
	available from the interpreter as:
		CleanRotMat( mat )
*/
void glCleanRotMat(double mat_data[16], double a[4][4])
{
  float s;
  int i;
  memcpy(a, (void *)mat_data, sizeof(double) * 16);

  for (i=0;i<3;i++) a[i][3]=a[3][i]=0.0;
  a[3][3]=1.0;

  for (i=0,s=0.0;i<3;i++) s+=a[0][i]*a[0][i];
  s=sqrt(s);
  for (i=0;i<3;i++) a[0][i]/=s;	/* first row normalized */

  a[2][0]=a[0][1]*a[1][2]-a[0][2]*a[1][1];
  a[2][1]=a[0][2]*a[1][0]-a[0][0]*a[1][2];
  a[2][2]=a[0][0]*a[1][1]-a[0][1]*a[1][0];
  for (i=0,s=0.0;i<3;i++) s+=a[2][i]*a[2][i];
  s=sqrt(s);
  for (i=0;i<3;i++) a[2][i]/=s;	/* third row orthonormal to first */

  a[1][0]=a[2][1]*a[0][2]-a[2][2]*a[0][1];
  a[1][1]=a[2][2]*a[0][0]-a[2][0]*a[0][2];
  a[1][2]=a[2][0]*a[0][1]-a[2][1]*a[0][0];
  for (i=0,s=0.0;i<3;i++) s+=a[1][i]*a[1][i];
  s=sqrt(s);
  for (i=0;i<3;i++) a[1][i]/=s;	/* second row orthonormal to 1,3 */
}


/* just a rewriting of GLUTSOLIDSPHERE by guillaume vareille */
#ifndef __APPLE__
static GLUquadricObj * quadObj = NULL ;
#endif // __APPLE__
void extractedGlutSolidSphere ( GLdouble radius , GLint slices , GLint stacks, int insideout )
{
  /* code extracted from glut_shape.c */
  /* printf ( "guillaume's extractedGlutSolidSphere" ) ; */

#ifdef __APPLE__
  static GLUquadricObj * quadObj = NULL ;
#endif // __APPLE__

  if ( ! quadObj ) 
  {
    quadObj = gluNewQuadric ( ) ;
    if ( ! quadObj )
    {
      printf ( "Can't allocate memory for extractedGlutSolidSphere" ) ;
      return ;
    }
  }

  if ( insideout == 0 )
  {
    gluQuadricOrientation ( quadObj , GLU_OUTSIDE ) ;
  }
  else
  {
    gluQuadricOrientation ( quadObj , GLU_INSIDE ) ;
  }

  gluQuadricDrawStyle ( quadObj , GLU_FILL ) ;
  
  gluQuadricNormals ( quadObj , GLU_SMOOTH ) ;
  /* If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
  gluSphere ( quadObj , radius , slices , stacks ) ;

#ifdef __APPLE__
  gluDeleteQuadric ( quadObj ) ;
  quadObj = NULL ;
#endif // __APPLE__

}
/* end of modified GLUTSOLIDSPHERE by guillaume vareille */


void solidCylinder ( GLdouble radiusBase ,
					 GLdouble radiusTop ,
                     GLdouble height ,
                     GLint slices ,
                     GLint stacks ,
                     int insideout )
{
  /* printf ( "guillaume's solidCylinder" ) ; */
  /* by guillaume vareille (inspired by GLUTSOLIDSPHERE) */

#ifdef __APPLE__
  static GLUquadricObj * quadObj = NULL ;
#endif // __APPLE__

  if ( ! quadObj ) 
  {
    quadObj = gluNewQuadric ( ) ;
    if ( ! quadObj )
    {
      printf ( "Can't allocate memory for extractedGlutSolidSphere" ) ;
      return ;
    }
  }

  if ( insideout == 0 )
  {
    gluQuadricOrientation ( quadObj , GLU_OUTSIDE ) ;
  }
  else
  {
    gluQuadricOrientation ( quadObj , GLU_INSIDE ) ;
  }

  gluQuadricDrawStyle ( quadObj , GLU_FILL ) ;

  gluQuadricNormals ( quadObj , GLU_SMOOTH ) ;
  /* If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
  gluCylinder ( quadObj , radiusBase , radiusTop , height, slices , stacks ) ;

#ifdef __APPLE__
  gluDeleteQuadric ( quadObj ) ;
  quadObj = NULL ;
#endif // __APPLE__

}


/*************
  Compute vector v, normal to the triangle (p1,p2,p3) assuming that the order
  of the points p1,p2,p3 provides the face's orientation
**************/
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

#include <string.h>


/*

 Computes the vector normal to each triangle using
 triangle_normal. The resulting normals are returned
 in a m*3 array of floats.
*/

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
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v1, i/3, lenv[0]);
	  return 0;
	}
      v2 = t_data[i+1];
      if ( v2 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v2, i/3, lenv[0]);
	  return 0;
	}
      v3 = t_data[i+2];
      if ( v3 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v3, i/3, lenv[0]);
	  return 0;
	}
      
      triangle_normal( &v_data[v1*3], &v_data[v2*3], &v_data[v3*3], 
		       &trinorm[i] );
    }
  return 1;
}

/**
Computes the vector normal to each triangle (face) using
triangle_normal. The normals for each vertex are obtained by 
summing up the faces normals of each triangle this vertex belongs to. The 
resulting normals are returned in a n*3 array of floats.
**/
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
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v1, i/3, lenv[0]);
	  return 0;
	}
      v2 = t_data[i+1];
      if ( v2 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v2, i/3, lenv[0]);
	  return 0;
	}
      v3 = t_data[i+2];
      if ( v3 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v3, i/3, lenv[0]);
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
	  for (j=0; j<3; j++) vnorm[i*3 + j] = 0.0;
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
	  for (k=0; k<3; k++) vnorm[i*3 + k] /= tric[i];
	}
      /*printf("free tric at %p\n", tric);*/
      free(tric);
      free(trinorm);
      return 1;
}

/**
The face and vertex normals are computed.
**/
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
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v1, i/3, lenv[0]);
	  return 0;
	}
      v2 = t_data[i+1];
      if ( v2 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v2, i/3, lenv[0]);
	  return 0;
	}
      v3 = t_data[i+2];
      if ( v3 >= lenv[0]) 
	{
	  fprintf(stderr, "Error: Coordinates index %d in face %d out of range %d\n", v3, i/3, lenv[0]);
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
      for (j=0; j<3; j++) vnorm[i*3 + j] = 0.0;
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
      for (k=0; k<3; k++) vnorm[i*3 + k] /= tric[i];
    }
  /*printf("free tric at %p\n", tric);*/
  free(tric);
  return 1;
}

/*
   WARNING this function is still experimental and has not yet being fully
	   tested!
   Build a displaylist for a set of indexed GL geometries (all but GL_POINTS).
   Indexed geometries are specified using a sequence of vertices (n*3 floats)
   and a sequence on m*p integers specifying how to connect the vertices.
   - All parts (faces or lines) can have exactly length p (p vertices) or can
     be of length < p in which case the list is terminated by -1.
   - Sets of lines of length 2, sets of triangles and sets of quads are drawn
     without sending glBegin( ... ) / glEnd () for each primitive. This is more
     efficient but has the draw back that the picking mechanism can no more
     distinguish between different parts. For instance, in order to get 
     pickable triangles one should use GL_POLYGON as the first argument rather
     than GL_TRIANGLES.
   - All arguments are named and some are optional.
   - Lighting is enable only if normals are specified and their number is
     either 1, number of index lists or number of vertices. The normal binding
     mode (OVERALL, PER_PART, PER_VERTEX) is infered from the number of 
     normals available.
   - When no normals are given, lighting is disabled and the polygon mode
     is set to lines.
   - Front and back polygon properties can be specified optionally. They have
     to be a sequence of 5 sequences of properties: one each of the material
     property component: GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_EMISSION,
		         GL_SHININESS
     * The first four property lists have to be (possibly empty) 4-sequences of
     floating point values ranging from 0.0 to 1.0 (RGBA).
     * The fifth property (GL_SHININESS) is a (possibly empty) sequence of
     floating points values ranging from 0.0 to 128.0.
     * property binding modes are selected automatically and separately for
     each property, based on the number of properties available and the number
     of vertices and parts (see normals bind mode).

   Required arguments:
      type       :    GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_POLYGON,
                      GL_QUADS, GL_QUAD_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP,
		      GL_TRIANGLE_FAN
      coordinates:    sequence of shape n*3 for n vertices (float or double)
      indices    :    sequence of shape m*3 for m triangles (int)

   Optional arguments:
      normals:        sequence of p*3 (float or double).
                      If p==n normals will be bound by vertex
                      If p==m normals will be bound by face
                      If p==1 normal set OVERALL
		      else lighting is turned off, GL_LINE is used ?
      frontMaterial:  sequence of p*4 float (0.0-1.0)
      backMaterial:   sequence of p*4 float (0.0-1.0)
      frontMatBind:   binding mode for front material
      backMatBind:    binding mode for back material
      frontAndBack:   int = 0 or 1 to use the front properties for back facing
                      polygons.
      texIndices:     texture indices n * 1,2,3 or 4 for n vertices 

03 '99: Remove overall coloring from display list (has to be done outside)
03 '99: Added color memory to minimize context switches

TODO: materials should be indexed too
*/
static short isNewColor(float *c, int assignColorToStatic )
{
  static float col[4];
  if (!c) {
    col[0] = col[1] = col[2] = col[3] = -1.0;
    return 0;
  } else {
    if (fabs(c[0]-col[0]) < 0.0001 && fabs(c[1]-col[1]) < 0.0001 && 
	fabs(c[2]-col[2]) < 0.0001 && fabs(c[3]-col[3]) < 0.0001) {
      return 0;
    }
#ifdef DEBUG
    printf("new color %f %f %f %f\n", c[0],c[1],c[2],c[3]);
#endif
		if (assignColorToStatic)
		{
	    col[0] = c[0];
	    col[1] = c[1];
	    col[2] = c[2];
	    col[3] = c[3];
		}
    return 1;
  }
}

static short isNewMaterial( int face, int prop, float *c, int assignColorToStatic )
{
  static float col[2][5][4];
  int f,i,j,k;
  if (!c) 
  {
    for (i=0;i<2;i++)
      for (j=0;j<5;j++)
	      for (k=0;k<4;k++)
	        col[i][j][k] = -1.0;
    return 0;
  }
  else
  {
    f = (face==GL_FRONT) ? 0:1;
    if (fabs(c[0]-col[f][prop][0]) < 0.0001 &&
	    fabs(c[1]-col[f][prop][1]) < 0.0001 && 
	    fabs(c[2]-col[f][prop][2]) < 0.0001 &&
	    fabs(c[3]-col[f][prop][3]) < 0.0001)
		{
      return 0;
  	}
#ifdef DEBUG
    printf("new material %d %d %f %f %f %f\n", face, prop, c[0],c[1],c[2],c[3]);
#endif
		if (assignColorToStatic)
		{
	    col[f][prop][0] = c[0];
	    col[f][prop][1] = c[1];
	    col[f][prop][2] = c[2];
	    col[f][prop][3] = c[3];
		}
    return 1;
  }
}

static void reapplyMaterial(
		int faceIndex ,
		float * frontMaterial [ 5 ] ,
		float * backMaterial [ 5 ] ,
		int frontMatBind [ 5 ] , 
		int backMatBind [ 5 ] ,
		int frontAndBack 
		)
{
	int k , ii , face ;
  int PER_VERTEX = 11 ;
  int propConst [ ] = { GL_AMBIENT , GL_DIFFUSE , GL_EMISSION , GL_SPECULAR , GL_SHININESS } ;
	int faceIndex4 = faceIndex * 4 ;

	if (frontMaterial)
	{
	  if (!frontAndBack)
			face = GL_FRONT;
	  else 
			face = GL_FRONT_AND_BACK;
		for (k=0; k<5; k++) 
		{
			if (frontMatBind[k] == PER_VERTEX)
			{
				if (k == 4) ii = faceIndex;
				else ii = faceIndex4;
			  glMaterialfv( (GLenum)face, (GLenum)propConst[k], &frontMaterial[k][ii] );
			}
		}
	}
	
  if (backMaterial && !frontAndBack)
	{
		for (k=0; k<5; k++) 
		{
			if (backMatBind[k] == PER_VERTEX)
			{
				if (k == 4) ii = faceIndex;
				else ii = faceIndex4;
			  glMaterialfv( (GLenum)GL_BACK, (GLenum)propConst[k], &backMaterial[k][ii] );
			}
		}
	}
}


static float colorDistance ( float aColorA [ 3 ] ,  float aColorB [ 3 ] )
{
	/* from "http://www.compuphase.com/cmetric.htm" */
  float r , g , b ;
  float rmean = ( aColorA [ 0 ] + aColorB [ 0 ] ) / 2 ;
  r = aColorA [ 0 ] - aColorB [ 0 ] ;
  g = aColorA [ 1 ] - aColorB [ 1 ] ;
  b = aColorA [ 2 ] - aColorB [ 2 ] ;
  return   ( 2 + rmean ) * r * r 
         + ( 4 * g * g )
         + ( 3 - rmean )  * b * b ;
}


static float isMultiColorDuplet ( 
		int indexA , 
		int indexB ,
		float * frontMaterial [ 5 ] ,
		float * backMaterial [ 5 ] ,
		int frontMatBind [ 5 ] , 
		int backMatBind [ 5 ] ,
		int frontAndBack )
{
	/*
	 * 0 : both are of the same color
	 * 1 : different colors
	 */

	int PER_VERTEX = 11 ;
	int i ;
	float * lA ;
	float * lB ;
	
	for ( i = 0 ; i < 5 ; i ++ )
	{
		if ( frontMatBind [ i ] == PER_VERTEX ) 
		{
			lA = & frontMaterial [ i ] [ indexA ] ;
			lB = & frontMaterial [ i ] [ indexB ] ;
			if ( ( lA[0] != lB[0] ) || ( lA[1] != lB[1] ) || ( lA[2] != lB[2] ) || ( lA[3] != lB[3] ) )
			{
				return colorDistance ( lA , lB ) ;
			}
		}
		if ( ! frontAndBack )
		{
			if ( backMatBind [ i ] == PER_VERTEX ) 
			{
		    lA = & backMaterial [ i ] [ indexA ] ;
				lB = & backMaterial [ i ] [ indexB ] ;
		    if ( ( lA[0] != lB[0] ) || ( lA[1] != lB[1] ) || ( lA[2] != lB[2] ) || ( lA[3] != lB[3] ) )
		    {
		    	return colorDistance ( lA , lB ) ;
		    }
			}
		}
	}
	return 0 ;
}


static int isMultiColorTriplet ( 
		int indexA , 
		int indexB ,
		int indexC ,
		float * frontMaterial [ 5 ] ,
		float * backMaterial [ 5 ] ,
		int frontMatBind [ 5 ] , 
		int backMatBind [ 5 ] ,
		int frontAndBack )
{
	/*
	 * 0 : all the same color
	 * 1 : A is different
	 * 2 : B is different
	 * 3 : C is different
	 * 4 : all different
	 */
 
	float lMultiColorAB ;
	float lMultiColorBC ;
	float lMultiColorCA ;
	lMultiColorAB = isMultiColorDuplet ( indexA , indexB , 
	                                     frontMaterial , backMaterial , 
	                                     frontMatBind , backMatBind ,
	                                     frontAndBack ) ;
	lMultiColorBC = isMultiColorDuplet ( indexB , indexC ,
	                                     frontMaterial , backMaterial , 
	                                     frontMatBind , backMatBind ,
	                                     frontAndBack ) ;
	if ( lMultiColorAB == 0. )
	{	/* A B are the same */
		if ( lMultiColorBC == 0. )
		{ /* B C are the same */
			return 0 ; /* all the same */
		}
		else
		{ /* B C are different */
			return 3 ; /* C is different */
		}
	}
    else 
    {	/* A B are different */
    	if ( lMultiColorBC == 0. )
		{ /* B C are the same */
		    return 1 ; /* A is different */
		}
		else
		{ /* B C are different */
			lMultiColorCA = isMultiColorDuplet ( indexC , indexA ,
			                                     frontMaterial , backMaterial , 
			                                     frontMatBind , backMatBind ,
			                                     frontAndBack ) ;

			if ( lMultiColorCA == 0. )
			{ /* C A are the same */
				return 2 ; /* B is different */
			}
			else
			{ /* C A are different */
				
				if (lMultiColorAB < lMultiColorBC)
				{
					if (lMultiColorAB < lMultiColorCA)
					{
						return 43 ; /* ie C is different */
					}
					else
					{
						return 42 ; /* ie B is different */
					}					
				}
				else
				{
					if (lMultiColorBC < lMultiColorCA)
					{
						return 41 ; /* ie A is different */
					}
					else
					{
						if ( (lMultiColorBC == lMultiColorCA) && (lMultiColorAB == lMultiColorBC) )
						{
							return 4 ; /* all different */
						}
						else
						{
							return 42 ; /* ie B is different */
						}
					}					
				}
			}
		}
	}
}


GLuint glDrawIndexedGeom(
       int type, 
       float *coordinates, int lencoord,
			 int *indices, int lenind[2], 
			 float * normals, int lennorm[2],
			 float *texIndices, int lentexind[2],
			 float *frontMaterial[5], int lenfm[5],
			 float *backMaterial[5], int lenbm[5],
			 int frontMatBind[5], int backMatBind[5],
			 int frontAndBack,  int noLightCol,
			 int sharpColorBoundaries, /*0: no sharp - 1: sharp with colordistance - 2: sharp without colordistance*/
			 int preventIntelBug
			 ,int * highlight, int lenhighlight
			 )
{
  int  freeFrontMatBind=0, freeBackMatBind=0;
  int i, j, k, l, v, fixed = 0, face, normBinding,
      NONE = -1, OVERALL = 10, PER_VERTEX = 11, PER_PART = 12,
      propConst[] = { GL_AMBIENT, GL_DIFFUSE, GL_EMISSION, GL_SPECULAR,
		      GL_SHININESS };
  int i4, v3, v4, ii, jj;

  int lPreviousV3;
  float lVx, lVy, lVz;

  int lSharpColorBoundariesTriangles, lMultiColor, lFullPrevention ;

  int lIndexA ;
  int lIndexB ;
  int lIndexC ;
  int lIndexA3 ;
  int lIndexB3 ;
  int lIndexC3 ;
  int lIndexA4 ;
  int lIndexB4 ;
  int lIndexC4 ;

  float lCenterEdgeAB [ 3 ] ;
  float lCenterEdgeBC [ 3 ] ;
  float lCenterEdgeCA [ 3 ] ;
  float lNormalEdgeAB [ 3 ] ;
  float lNormalEdgeBC [ 3 ] ;
  float lNormalEdgeCA [ 3 ] ;
  float lCenterABC [ 3 ] ;
  float lCenterNormalABC [ 3 ] ;
  float lCenterSegment [ 3 ] ;
  float lNormalSegment [ 3 ] ;

  int lHighlightState ;
  int lhighlight ;
  
  GLenum errcode;

  if ( ( sharpColorBoundaries ) 
    && ( lenind [ 1 ] == 3 ) 
    && ( ! texIndices ) 
    && ( frontMatBind ) 
    && ( ( frontMatBind [ 0 ] == PER_VERTEX ) 
      || ( frontMatBind [ 1 ] == PER_VERTEX )
      || ( frontMatBind [ 2 ] == PER_VERTEX )
      || ( frontMatBind [ 3 ] == PER_VERTEX )
      || ( frontMatBind [ 4 ] == PER_VERTEX )
      || ( ( ! frontAndBack ) 
        && ( backMatBind ) 
        && ( ( backMatBind [ 0 ] == PER_VERTEX )
		  || ( backMatBind [ 1 ] == PER_VERTEX )
		  || ( backMatBind [ 2 ] == PER_VERTEX )
		  || ( backMatBind [ 3 ] == PER_VERTEX )
		  || ( backMatBind [ 4 ] == PER_VERTEX ) 
		   )
		 )
	   )
     )
  {
  	lSharpColorBoundariesTriangles = 1 ;
  }
  else
  {
  	lSharpColorBoundariesTriangles = 0 ;
  	lMultiColor = -1 ;
  }

	if ( preventIntelBug && lSharpColorBoundariesTriangles )
	{
		lFullPrevention = 1;
	}
	else
	{
		lFullPrevention = 0 ;
	}
	
  if (texIndices)
  {
		if (lentexind[0] != lencoord)
		{
		  fprintf(stderr, "ERROR in glDrawIndexedGeom: Number of texture indices(%d) doesn't match number of vertices(%d)\n", lentexind[0], lencoord);
		  return 0;
		}
  }

  if (normals) 
  {
		if (lennorm[0] == lencoord) 
			normBinding = PER_VERTEX;
    else if (lennorm[0] == lenind[0]) 
			normBinding = PER_PART;
    else if (lennorm[0] == 1) 
    	normBinding = OVERALL;
    else normBinding = NONE;
  }
  else normBinding = NONE;
  
  /* check front material binding parameter */
  if (frontMaterial) 
  {
		if (!frontMatBind) 
		{
	  	frontMatBind = (int *)malloc(5*sizeof(int));
	  	freeFrontMatBind = 1; 
	  	for (i=0; i<5; i++) 
	    {
	      if (lenfm[i] == lencoord)
					frontMatBind[i]=PER_VERTEX;
	      else if (lenfm[i] == lenind[0])
					frontMatBind[i]=PER_PART;
	      else if (lenfm[i] == 1) 
					frontMatBind[i]=OVERALL;
	      else lenfm[i] = 0;
	    }
		}
  } 

  /* check back material binding parameter */
  if (backMaterial)
  {
    if (!backMatBind)
		{
		  backMatBind = (int *)malloc(5*sizeof(int));
		  freeBackMatBind = 1; 
		  for (i=0; i<5; i++)
		  {
		    if (lenbm[i] == lencoord)
					backMatBind[i]=PER_VERTEX;
		    else if (lenbm[i] == lenind[0])
					backMatBind[i]=PER_PART;
		    else if (lenbm[i] == 1)
					backMatBind[i]=OVERALL;
		    else lenbm[i] = 0;
		  }
		}
  } 

  if (!frontAndBack)
		face = GL_FRONT;
  else 
		face = GL_FRONT_AND_BACK;

	if (normBinding == OVERALL ) 
	{ 
		glNormal3fv( &normals[0] );  /* Overall Normal */
	}

	if (type==GL_LINES)
	{
	  glDisable(GL_LIGHTING);
	} 

	if ( lenhighlight > 0 )
	{
		glStencilFunc ( GL_ALWAYS, 0, 1 ) ;
	}
  lHighlightState = 0 ;

  if (type==GL_LINES || type==GL_TRIANGLES || type==GL_QUADS)
	{
		fixed = 1;
		glBegin((GLenum)type);   /* fixed length geom's ==> vertices cannot be picked */
	}
  /* initialize color memory */
  isNewColor(NULL,1);
  isNewMaterial(0,0,NULL,1);
  l = lenind[1];
  /* loop over faces */
  for (i=0; i<lenind[0]; i++)
	{
    i4 = i * 4 ; 
    if (normBinding == PER_PART)
    {
    	glNormal3fv( &normals[i*3] ); /* PER_PART */
    }
      
    /* set PER_PART color/material properties */
	
	  if (frontMaterial)
    {
      if (frontMatBind[noLightCol] == PER_PART)
      {
				if ( isNewColor(&frontMaterial[noLightCol][i4],1) )
		  	{
		    	glColor4fv( &frontMaterial[noLightCol][i4] );
		  	}
      }
      if ( preventIntelBug == 0 )
      {
	      for (j=0; j<5; j++)
				{
		  		if (frontMatBind[j] == PER_PART)
		  		{
						if (j == 4) ii = i;
						else ii = i4; 
		    		if ( preventIntelBug || isNewMaterial( face, j, &frontMaterial[j][ii],1) )
		      	{
							glMaterialfv( (GLenum)face, (GLenum)propConst[j], &frontMaterial[j][ii] );
		      	}
		  		}
				}
			}
    }
  	if ( ( preventIntelBug == 0 ) && backMaterial && !frontAndBack)
    {
      for (j=0; j<5; j++) 
			{
			  if (backMatBind[j] == PER_PART)
			  {
					if (j == 4) ii = i;
					else ii = i4; 
			    if ( preventIntelBug || isNewMaterial( GL_BACK, j, &backMaterial[j][ii],1) )
			    {
						glMaterialfv( GL_BACK, (GLenum)propConst[j], &backMaterial[j][ii] );
			    }
			  }
			}
		}
      
		if ( lSharpColorBoundariesTriangles ) 
		{
      lIndexA = indices [ i * l + 0 ] ;
      lIndexB = indices [ i * l + 1 ] ;
      lIndexC = indices [ i * l + 2 ] ;
      lIndexA4 = 4 * lIndexA ;
      lIndexB4 = 4 * lIndexB ;
      lIndexC4 = 4 * lIndexC ;
      lMultiColor = isMultiColorTriplet (
		    lIndexA4 , 
		    lIndexB4 ,
		    lIndexC4 ,
			frontMaterial ,
			backMaterial ,
			frontMatBind ,
			backMatBind ,
			frontAndBack ) ;

			if ( lMultiColor != 0 )
		  {
		  	if ( ( lMultiColor > 40 ) && ( sharpColorBoundaries == 2 ) )
		  	{
		  		lMultiColor = 4 ;
		  	}
		  	
	      lIndexA3 = 3 * lIndexA ;
	      lIndexB3 = 3 * lIndexB ;
	      lIndexC3 = 3 * lIndexC ;
			  if ( lMultiColor == 1 )
			  {
				  lCenterEdgeAB[0] = .5 * ( (&coordinates[lIndexA3])[0] + (&coordinates[lIndexB3])[0] ) ;
				  lCenterEdgeAB[1] = .5 * ( (&coordinates[lIndexA3])[1] + (&coordinates[lIndexB3])[1] ) ;
			    lCenterEdgeAB[2] = .5 * ( (&coordinates[lIndexA3])[2] + (&coordinates[lIndexB3])[2] ) ;
				  lCenterEdgeCA[0] = .5 * ( (&coordinates[lIndexC3])[0] + (&coordinates[lIndexA3])[0] ) ;
				  lCenterEdgeCA[1] = .5 * ( (&coordinates[lIndexC3])[1] + (&coordinates[lIndexA3])[1] ) ;
			    lCenterEdgeCA[2] = .5 * ( (&coordinates[lIndexC3])[2] + (&coordinates[lIndexA3])[2] ) ;
			    if (normBinding == PER_VERTEX)
			    {
					  lNormalEdgeAB[0] = .5 * ( (&normals[lIndexA3])[0] + (&normals[lIndexB3])[0] ) ;
					  lNormalEdgeAB[1] = .5 * ( (&normals[lIndexA3])[1] + (&normals[lIndexB3])[1] ) ;
					  lNormalEdgeAB[2] = .5 * ( (&normals[lIndexA3])[2] + (&normals[lIndexB3])[2] ) ;
					  lNormalEdgeCA[0] = .5 * ( (&normals[lIndexC3])[0] + (&normals[lIndexA3])[0] ) ;
					  lNormalEdgeCA[1] = .5 * ( (&normals[lIndexC3])[1] + (&normals[lIndexA3])[1] ) ;
					  lNormalEdgeCA[2] = .5 * ( (&normals[lIndexC3])[2] + (&normals[lIndexA3])[2] ) ;
			    }
			    
			  }
			  else if ( lMultiColor == 2 )
			  {
				  lCenterEdgeAB[0] = .5 * ( (&coordinates[lIndexA3])[0] + (&coordinates[lIndexB3])[0] ) ;
				  lCenterEdgeAB[1] = .5 * ( (&coordinates[lIndexA3])[1] + (&coordinates[lIndexB3])[1] ) ;
			    lCenterEdgeAB[2] = .5 * ( (&coordinates[lIndexA3])[2] + (&coordinates[lIndexB3])[2] ) ;
				  lCenterEdgeBC[0] = .5 * ( (&coordinates[lIndexB3])[0] + (&coordinates[lIndexC3])[0] ) ;
				  lCenterEdgeBC[1] = .5 * ( (&coordinates[lIndexB3])[1] + (&coordinates[lIndexC3])[1] ) ;
			    lCenterEdgeBC[2] = .5 * ( (&coordinates[lIndexB3])[2] + (&coordinates[lIndexC3])[2] ) ;
			    if (normBinding == PER_VERTEX)
			    {
					  lNormalEdgeAB[0] = .5 * ( (&normals[lIndexA3])[0] + (&normals[lIndexB3])[0] ) ;
					  lNormalEdgeAB[1] = .5 * ( (&normals[lIndexA3])[1] + (&normals[lIndexB3])[1] ) ;
					  lNormalEdgeAB[2] = .5 * ( (&normals[lIndexA3])[2] + (&normals[lIndexB3])[2] ) ;
					  lNormalEdgeBC[0] = .5 * ( (&normals[lIndexB3])[0] + (&normals[lIndexC3])[0] ) ;
					  lNormalEdgeBC[1] = .5 * ( (&normals[lIndexB3])[1] + (&normals[lIndexC3])[1] ) ;
					  lNormalEdgeBC[2] = .5 * ( (&normals[lIndexB3])[2] + (&normals[lIndexC3])[2] ) ;
			    }

			  }
			  else if ( lMultiColor == 3 )
			  {
				  lCenterEdgeBC[0] = .5 * ( (&coordinates[lIndexB3])[0] + (&coordinates[lIndexC3])[0] ) ;
				  lCenterEdgeBC[1] = .5 * ( (&coordinates[lIndexB3])[1] + (&coordinates[lIndexC3])[1] ) ;
			    lCenterEdgeBC[2] = .5 * ( (&coordinates[lIndexB3])[2] + (&coordinates[lIndexC3])[2] ) ;
				  lCenterEdgeCA[0] = .5 * ( (&coordinates[lIndexC3])[0] + (&coordinates[lIndexA3])[0] ) ;
				  lCenterEdgeCA[1] = .5 * ( (&coordinates[lIndexC3])[1] + (&coordinates[lIndexA3])[1] ) ;
			    lCenterEdgeCA[2] = .5 * ( (&coordinates[lIndexC3])[2] + (&coordinates[lIndexA3])[2] ) ;
			    if (normBinding == PER_VERTEX)
			    {
					  lNormalEdgeBC[0] = .5 * ( (&normals[lIndexB3])[0] + (&normals[lIndexC3])[0] ) ;
					  lNormalEdgeBC[1] = .5 * ( (&normals[lIndexB3])[1] + (&normals[lIndexC3])[1] ) ;
					  lNormalEdgeBC[2] = .5 * ( (&normals[lIndexB3])[2] + (&normals[lIndexC3])[2] ) ;
					  lNormalEdgeCA[0] = .5 * ( (&normals[lIndexC3])[0] + (&normals[lIndexA3])[0] ) ;
					  lNormalEdgeCA[1] = .5 * ( (&normals[lIndexC3])[1] + (&normals[lIndexA3])[1] ) ;
					  lNormalEdgeCA[2] = .5 * ( (&normals[lIndexC3])[2] + (&normals[lIndexA3])[2] ) ;
			    }

			  }
			  else if ( ( lMultiColor == 4 ) || ( lMultiColor > 40 ) )
			  {
				  lCenterEdgeAB[0] = .5 * ( (&coordinates[lIndexA3])[0] + (&coordinates[lIndexB3])[0] ) ;
				  lCenterEdgeAB[1] = .5 * ( (&coordinates[lIndexA3])[1] + (&coordinates[lIndexB3])[1] ) ;
			    lCenterEdgeAB[2] = .5 * ( (&coordinates[lIndexA3])[2] + (&coordinates[lIndexB3])[2] ) ;
				  lCenterEdgeBC[0] = .5 * ( (&coordinates[lIndexB3])[0] + (&coordinates[lIndexC3])[0] ) ;
				  lCenterEdgeBC[1] = .5 * ( (&coordinates[lIndexB3])[1] + (&coordinates[lIndexC3])[1] ) ;
			    lCenterEdgeBC[2] = .5 * ( (&coordinates[lIndexB3])[2] + (&coordinates[lIndexC3])[2] ) ;
				  lCenterEdgeCA[0] = .5 * ( (&coordinates[lIndexC3])[0] + (&coordinates[lIndexA3])[0] ) ;
				  lCenterEdgeCA[1] = .5 * ( (&coordinates[lIndexC3])[1] + (&coordinates[lIndexA3])[1] ) ;
			    lCenterEdgeCA[2] = .5 * ( (&coordinates[lIndexC3])[2] + (&coordinates[lIndexA3])[2] ) ;
			    if (normBinding == PER_VERTEX)
			    {
					  lNormalEdgeAB[0] = .5 * ( (&normals[lIndexA3])[0] + (&normals[lIndexB3])[0] ) ;
					  lNormalEdgeAB[1] = .5 * ( (&normals[lIndexA3])[1] + (&normals[lIndexB3])[1] ) ;
					  lNormalEdgeAB[2] = .5 * ( (&normals[lIndexA3])[2] + (&normals[lIndexB3])[2] ) ;
					  lNormalEdgeBC[0] = .5 * ( (&normals[lIndexB3])[0] + (&normals[lIndexC3])[0] ) ;
					  lNormalEdgeBC[1] = .5 * ( (&normals[lIndexB3])[1] + (&normals[lIndexC3])[1] ) ;
					  lNormalEdgeBC[2] = .5 * ( (&normals[lIndexB3])[2] + (&normals[lIndexC3])[2] ) ;
					  lNormalEdgeCA[0] = .5 * ( (&normals[lIndexC3])[0] + (&normals[lIndexA3])[0] ) ;
					  lNormalEdgeCA[1] = .5 * ( (&normals[lIndexC3])[1] + (&normals[lIndexA3])[1] ) ;
					  lNormalEdgeCA[2] = .5 * ( (&normals[lIndexC3])[2] + (&normals[lIndexA3])[2] ) ;
			    }
			    
			    if ( lMultiColor == 4 )
			    {
			      lCenterABC[0] = .3333333333333333333333333333333 * 
					                  (  (&coordinates[lIndexA3])[0] 
					                   + (&coordinates[lIndexB3])[0] 
					                   + (&coordinates[lIndexC3])[0] ) ;
			      lCenterABC[1] = .3333333333333333333333333333333 * 
					                  (  (&coordinates[lIndexA3])[1] 
					                   + (&coordinates[lIndexB3])[1] 
					                   + (&coordinates[lIndexC3])[1] ) ;
			      lCenterABC[2] = .3333333333333333333333333333333 * 
					                  (  (&coordinates[lIndexA3])[2] 
					                   + (&coordinates[lIndexB3])[2] 
					                   + (&coordinates[lIndexC3])[2] ) ;
			      lCenterNormalABC[0] = .3333333333333333333333333333333 * 
									                (  (&normals[lIndexA3])[0] 
									                 + (&normals[lIndexB3])[0] 
									                 + (&normals[lIndexC3])[0] ) ;
			      lCenterNormalABC[1] = .3333333333333333333333333333333 * 
									                (  (&normals[lIndexA3])[1] 
									                 + (&normals[lIndexB3])[1] 
									                 + (&normals[lIndexC3])[1] ) ;
			      lCenterNormalABC[2] = .3333333333333333333333333333333 * 
									                (  (&normals[lIndexA3])[2] 
									                 + (&normals[lIndexB3])[2] 
									                 + (&normals[lIndexC3])[2] ) ;
			    }
				  else if ( lMultiColor == 41 )
				  {
					  lCenterSegment[0] = .5 * ( lCenterEdgeAB[0] + lCenterEdgeCA[0] ) ;
					  lCenterSegment[1] = .5 * ( lCenterEdgeAB[1] + lCenterEdgeCA[1] ) ;
				    lCenterSegment[2] = .5 * ( lCenterEdgeAB[2] + lCenterEdgeCA[2] ) ;
				    if (normBinding == PER_VERTEX)
				    {
						  lNormalSegment[0] = .5 * ( lNormalEdgeAB[0] + lNormalEdgeCA[0] ) ;
						  lNormalSegment[1] = .5 * ( lNormalEdgeAB[1] + lNormalEdgeCA[1] ) ;
						  lNormalSegment[2] = .5 * ( lNormalEdgeAB[2] + lNormalEdgeCA[2] ) ;
				    }
				  }
				  else if ( lMultiColor == 42 )
				  {
					  lCenterSegment[0] = .5 * ( lCenterEdgeAB[0] + lCenterEdgeBC[0] ) ;
					  lCenterSegment[1] = .5 * ( lCenterEdgeAB[1] + lCenterEdgeBC[1] ) ;
				    lCenterSegment[2] = .5 * ( lCenterEdgeAB[2] + lCenterEdgeBC[2] ) ;
				    if (normBinding == PER_VERTEX)
				    {
						  lNormalSegment[0] = .5 * ( lNormalEdgeAB[0] + lNormalEdgeBC[0] ) ;
						  lNormalSegment[1] = .5 * ( lNormalEdgeAB[1] + lNormalEdgeBC[1] ) ;
						  lNormalSegment[2] = .5 * ( lNormalEdgeAB[2] + lNormalEdgeBC[2] ) ;
				    }
				  }
				  else if ( lMultiColor == 43 )
				  {
					  lCenterSegment[0] = .5 * ( lCenterEdgeBC[0] + lCenterEdgeCA[0] ) ;
					  lCenterSegment[1] = .5 * ( lCenterEdgeBC[1] + lCenterEdgeCA[1] ) ;
				    lCenterSegment[2] = .5 * ( lCenterEdgeBC[2] + lCenterEdgeCA[2] ) ;
				    if (normBinding == PER_VERTEX)
				    {
						  lNormalSegment[0] = .5 * ( lNormalEdgeBC[0] + lNormalEdgeCA[0] ) ;
						  lNormalSegment[1] = .5 * ( lNormalEdgeBC[1] + lNormalEdgeCA[1] ) ;
						  lNormalSegment[2] = .5 * ( lNormalEdgeBC[2] + lNormalEdgeCA[2] ) ;
				    }
				  }
			    
			  }
		  }
    }
    else if ( ! fixed ) 
		{
		  glPushName(i);
  	  glBegin((GLenum)type);
		}

//############################################
		if ( lenhighlight > 0 )
		{
			lhighlight = 1 ;
			for ( j = 0 ; j < l ; j ++ )
			{
				v = indices [ i * l + j ] ;
				if ( highlight [ v ] == 0 )
				{
					lhighlight = 0 ;
					break ;
				}
			}

			if ( lHighlightState == 0 )
			{
				if ( lhighlight == 1 )
				{
					if ( fixed )
					{
						glEnd ( ) ;
					}
	        glStencilFunc ( GL_ALWAYS , 1 , 1 ) ;
					lHighlightState = 1 ;
					if ( fixed )
					{
						glBegin ( ( GLenum ) type ) ;
					}
				}
			}
			else
			{
				if ( lhighlight == 0 )
				{
					if ( fixed )
					{
						glEnd ( ) ;
					}
	        glStencilFunc ( GL_ALWAYS , 0 , 1 ) ;
					lHighlightState = 0 ;
					if ( fixed )
					{
						glBegin ( ( GLenum ) type ) ;
					}
				}
			}
		}
//############################################


    /* loop over vertices in face */
    for (j=0; j<l; j++) 
		{

			if (preventIntelBug && ! lSharpColorBoundariesTriangles)
			{
			  if (frontMaterial)
		    {
		      for (jj=0; jj<5; jj++)
					{
			  		if (frontMatBind[jj] == PER_PART)
			  		{
							if (jj == 4) ii = i;
							else ii = i4; 
							glMaterialfv( (GLenum)face, (GLenum)propConst[jj], &frontMaterial[jj][ii] );
			  		}
					}
		    }
		  	if (backMaterial && !frontAndBack)
		    {
		      for (jj=0; jj<5; jj++) 
					{
					  if (backMatBind[jj] == PER_PART)
					  {
							if (jj == 4) ii = i;
							else ii = i4; 
							glMaterialfv( GL_BACK, (GLenum)propConst[jj], &backMaterial[jj][ii] );
					  }
					}
				}
			}

		  v = indices[i*l+j];
		  if (v < 0) break;
		  v3 = v * 3 ;
		  v4 = v * 4 ;
		  
		  if ( v >= lencoord ) 
		  {
	      fprintf(stderr, "ERROR in glDrawIndexedGeom: Coordinates index %d in face %d out of range %d\n", v, j, lencoord);
	      return 0;
		  }
		    
		  if (frontMaterial) 
			{
			  if ( ( (frontMatBind [ noLightCol ] == PER_VERTEX) && isNewColor( &frontMaterial[ noLightCol ][ v4 ] , 0) )
			    || ( (frontMatBind [ 0 ] == PER_VERTEX) && (preventIntelBug || isNewMaterial(face, 0, &frontMaterial[ 0 ][ v4 ], 0)) )
			    || ( (frontMatBind [ 1 ] == PER_VERTEX) && (preventIntelBug || isNewMaterial(face, 1, &frontMaterial[ 1 ][ v4 ], 0)) )
			    || ( (frontMatBind [ 2 ] == PER_VERTEX) && (preventIntelBug || isNewMaterial(face, 2, &frontMaterial[ 2 ][ v4 ], 0)) )
			    || ( (frontMatBind [ 3 ] == PER_VERTEX) && (preventIntelBug || isNewMaterial(face, 3, &frontMaterial[ 3 ][ v4 ], 0)) )
			    || ( (frontMatBind [ 4 ] == PER_VERTEX) && (preventIntelBug || isNewMaterial(face, 4, &frontMaterial[ 4 ][ v ], 0)) )
			  )
		    {
		    	if ( ( sharpColorBoundaries ) 
		      	  && ( (type == GL_LINES) || (l == 2) )
		      	  && ( j > 0 ) 
		      	   )
		      {
	      		lPreviousV3 = 3 * indices [ i * l + ( j - 1 ) ] ;
            lVx = .5 * ( (&coordinates[v3])[0] + (&coordinates[lPreviousV3])[0] ) ;
            lVy = .5 * ( (&coordinates[v3])[1] + (&coordinates[lPreviousV3])[1] ) ;
            lVz = .5 * ( (&coordinates[v3])[2] + (&coordinates[lPreviousV3])[2] ) ;
      	    glVertex3f ( lVx , lVy , lVz ) ;
      	    if ( (frontMatBind [ noLightCol ] == PER_VERTEX) && isNewColor( &frontMaterial[ noLightCol ][ v4 ],1 ) )
      	    {
			    	    glColor4fv( &frontMaterial[noLightCol][v4] );
      	    }
			  		for (k=0; k<5; k++) 
			    	{
			      	if (frontMatBind[k] == PER_VERTEX)
			      	{
								if (k == 4) ii = v;
								else ii = v4;
								if ( preventIntelBug || isNewMaterial( face, k, &frontMaterial[k][ii],1) )
							  {
							    glMaterialfv( (GLenum)face, (GLenum)propConst[k], &frontMaterial[k][ii] );
							  }
			      	}
			    	}
      	    glVertex3f ( lVx , lVy , lVz ) ;
		      }
	      	else if ( (frontMatBind [ noLightCol ] == PER_VERTEX) && isNewColor( &frontMaterial[ noLightCol ][ v4 ],1 ) )
	      	{
		    		glColor4fv( &frontMaterial[noLightCol][v4] );
	      	}
		    }

				if ( ! lFullPrevention )
				{
		  		for (k=0; k<5; k++) 
		    	{
		      	if (frontMatBind[k] == PER_VERTEX)
		      	{
							if (k == 4) ii = v;
							else ii = v4;
							if ( preventIntelBug || isNewMaterial( face, k, &frontMaterial[k][ii],1) )
						  {
						    glMaterialfv( (GLenum)face, (GLenum)propConst[k], &frontMaterial[k][ii] );
						  }
		      	}
		    	}
				}
			}

      if (backMaterial && ! frontAndBack && ! lFullPrevention )
			{
	  		for (k=0; k<5; k++)
		    {
		      if (backMatBind[k] == PER_VERTEX)
		      {
						if (k == 4) ii = v;
						else ii = v4;
						if ( preventIntelBug || isNewMaterial( GL_BACK, k, &backMaterial[k][ii],1) ) 
		  			{
		    			glMaterialfv( GL_BACK, (GLenum)propConst[k], &backMaterial[k][ii] );
		  			}
		      }
	    	}
			}

	  	if (texIndices)
	    {
	      switch(lentexind[1])
				{
					case 1: glTexCoord1f(texIndices[v]); break;
					case 2: glTexCoord2fv(&texIndices[v*2]); break;
					case 3: glTexCoord3fv(&texIndices[v3]); break;
					case 4: glTexCoord4fv(&texIndices[v4]); break;
				}
	    }

		  if ( (lSharpColorBoundariesTriangles==0) || (lMultiColor==0) )
		  {
		  	if (normBinding == PER_VERTEX)
		    {
		      if ( v >= lennorm[0] ) 
					{
			  		fprintf(stderr, "ERROR in glDrawIndexedGeom: Normal index %d in face %d out of range %d\n", v, j, lennorm[0]);
			  		return 0;
					}
		      glNormal3fv( &normals[v3] );
		    }

	    	if (lFullPrevention )
	    	  reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    glVertex3fv(&coordinates[v3]);
		  }
		  else if ( lMultiColor == 1 )
		  {
  	    if ( j == 0 )
  	    {
  	    	if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv( &normals[v3] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    glVertex3fv(&coordinates[v3]);
  	    }
  	    else /* if ( j == 1 ) as we break the for loop at the end */ 
  	    {
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
					glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv ( & normals [ lIndexB3 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    	glVertex3fv ( & coordinates [ lIndexB3 ] ) ;


		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    	glVertex3fv ( & coordinates [ lIndexB3 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv ( & normals [ lIndexC3 ] ) ;			    
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    glVertex3fv ( & coordinates [ lIndexC3 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
			    break ;
  	    }
		  }
		  else if ( lMultiColor == 2 )
		  {
  	    if ( j == 0 )
  	    {
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv ( & normals [ lIndexC3 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    	glVertex3fv ( & coordinates [ lIndexC3 ] ) ;
	  	    
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    	glVertex3fv ( & coordinates [ lIndexC3 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv ( & normals [ lIndexA3 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
					glVertex3fv ( & coordinates [ lIndexA3 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
  	    }
  	    else  /* if ( j == 1 ) as we break the for loop at the end */ 
  	    {
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv( &normals[v3] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    glVertex3fv(&coordinates[v3]);
			    break ;
  	    }
		  }
		  else if ( lMultiColor == 3 )
		  {
  	    if ( j == 0 )
  	    {
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
					glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv ( & normals [ lIndexA3 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    	glVertex3fv ( & coordinates [ lIndexA3 ] ) ;

		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    	glVertex3fv ( & coordinates [ lIndexA3 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv ( & normals [ lIndexB3 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    glVertex3fv ( & coordinates [ lIndexB3 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
  	    }
  	    else if ( j == 2 ) /* otherwise it won't be the right material */ 
  	    {
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
	    		if (normBinding == PER_VERTEX)
	    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
	  	    if (normBinding == PER_VERTEX)
	    			glNormal3fv( &normals[v3] ) ;
		    	if (lFullPrevention )
	    	    reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    glVertex3fv(&coordinates[v3]);
  	    }
		  }
		  else if ( lMultiColor == 4 )
		  {
  	    switch ( j )
  	    {
  	    	case 0:
		    		if (normBinding == PER_VERTEX)
		    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	      reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		break;
  	    	case 1:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		break;
  	    	case 2:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
						glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		break;
  	    }
  	    if (normBinding == PER_VERTEX)
	    		glNormal3fv( &normals[v3] ) ;
    	  if (lFullPrevention )
  	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    	glVertex3fv(&coordinates[v3]);

  	    if (normBinding == PER_VERTEX)
	    		glNormal3f ( lCenterNormalABC[0] , lCenterNormalABC[1] , lCenterNormalABC[2] ) ;
    	  if (lFullPrevention )
  	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
  	    glVertex3f ( lCenterABC[0] , lCenterABC[1] , lCenterABC[2] ) ;
  	    switch ( j )
  	    {
  	    	case 0:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
		    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		break;
  	    	case 1:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
		    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		break;
  	    	case 2:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
		    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		break;
				}
		  }
			else if ( lMultiColor == 41 )
			{
  	    switch ( j )
  	    {
  	    	case 0:
		    		if (normBinding == PER_VERTEX)
		    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
		    	  if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalSegment [ 0 ] , lNormalSegment [ 1 ] , lNormalSegment [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterSegment [ 0 ] , lCenterSegment [ 1 ] , lCenterSegment [ 2 ] ) ;
		  	    if (normBinding == PER_VERTEX)
			    		glNormal3fv( &normals[v3] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    	glVertex3fv(&coordinates[v3]);
			    	
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalSegment [ 0 ] , lNormalSegment [ 1 ] , lNormalSegment [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterSegment [ 0 ] , lCenterSegment [ 1 ] , lCenterSegment [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		break;
  	    	case 1:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		break;
  	    	case 2:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
						glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		break;
  	    }
  	    if (normBinding == PER_VERTEX)
	    		glNormal3fv( &normals[v3] ) ;
	    	if (lFullPrevention )
  	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    	glVertex3fv(&coordinates[v3]);

				if (j != 0)
				{

	  	    if (normBinding == PER_VERTEX)
		    		glNormal3f ( lNormalSegment[0] , lNormalSegment[1] , lNormalSegment[2] ) ;
		    	if (lFullPrevention )
    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	  	    glVertex3f ( lCenterSegment[0] , lCenterSegment[1] , lCenterSegment[2] ) ;
	  	    switch ( j )
	  	    {
	  	    	case 1:
			    		if (normBinding == PER_VERTEX)
		    			  glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
			    		if (normBinding == PER_VERTEX)
			    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
			    		break;
	  	    	case 2:
			    		if (normBinding == PER_VERTEX)
		    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
			    		if (normBinding == PER_VERTEX)
			    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
			    		break;
	  	    }
				}
			}
			else if ( lMultiColor == 42 )
			{
  	    switch ( j )
  	    {
  	    	case 0:
		    		if (normBinding == PER_VERTEX)
		    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		break;
  	    	case 1:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalSegment [ 0 ] , lNormalSegment [ 1 ] , lNormalSegment [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterSegment [ 0 ] , lCenterSegment [ 1 ] , lCenterSegment [ 2 ] ) ;
		  	    if (normBinding == PER_VERTEX)
			    		glNormal3fv( &normals[v3] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    	glVertex3fv(&coordinates[v3]);
			  
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalSegment [ 0 ] , lNormalSegment [ 1 ] , lNormalSegment [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterSegment [ 0 ] , lCenterSegment [ 1 ] , lCenterSegment [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		break;
  	    	case 2:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
						glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		break;
  	    }
  	    if (normBinding == PER_VERTEX)
	    		glNormal3fv( &normals[v3] ) ;
	    	if (lFullPrevention )
  	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    	glVertex3fv(&coordinates[v3]);

				if (j != 1)
				{
	  	    if (normBinding == PER_VERTEX)
		    		glNormal3f ( lNormalSegment[0] , lNormalSegment[1] , lNormalSegment[2] ) ;
		    	if (lFullPrevention )
	  	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	  	    glVertex3f ( lCenterSegment[0] , lCenterSegment[1] , lCenterSegment[2] ) ;
	  	    switch ( j )
	  	    {
	  	    	case 0:
			    		if (normBinding == PER_VERTEX)
		    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
			    		if (normBinding == PER_VERTEX)
			    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
			    		break;
	  	    	case 2:
			    		if (normBinding == PER_VERTEX)
		    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
			    		if (lFullPrevention )
	    	  		  reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
			    		if (normBinding == PER_VERTEX)
			    			glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
			    		if (lFullPrevention )
	    	  		 reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
			    		break;
	  	    }
				}
			}
			else if ( lMultiColor == 43 )
			{
  	    switch ( j )
  	    {
  	    	case 0:
		    		if (normBinding == PER_VERTEX)
		    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		break;
  	    	case 1:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
		    		break;
  	    	case 2:
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalSegment [ 0 ] , lNormalSegment [ 1 ] , lNormalSegment [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterSegment [ 0 ] , lCenterSegment [ 1 ] , lCenterSegment [ 2 ] ) ;

		  	    if (normBinding == PER_VERTEX)
			    		glNormal3fv( &normals[v3] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    	glVertex3fv(&coordinates[v3]);
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalSegment [ 0 ] , lNormalSegment [ 1 ] , lNormalSegment [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
		    		glVertex3f ( lCenterSegment [ 0 ] , lCenterSegment [ 1 ] , lCenterSegment [ 2 ] ) ;
		    		if (normBinding == PER_VERTEX)
	    			  glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
			    	if (lFullPrevention )
	    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
						glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
		    		break;
  	    }
  	    if ( normBinding == PER_VERTEX )
	    		glNormal3fv( &normals[v3] ) ;
	    	if (lFullPrevention )
  	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	    	glVertex3fv(&coordinates[v3]);

				if (j != 2)
				{
	  	    if (normBinding == PER_VERTEX)
		    		glNormal3f ( lNormalSegment[0] , lNormalSegment[1] , lNormalSegment[2] ) ;
		    	if (lFullPrevention )
    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
	  	    glVertex3f ( lCenterSegment[0] , lCenterSegment[1] , lCenterSegment[2] ) ;
	  	    switch ( j )
	  	    {
	  	    	case 0:
			    		if (normBinding == PER_VERTEX)
		    			  glNormal3f ( lNormalEdgeCA [ 0 ] , lNormalEdgeCA [ 1 ] , lNormalEdgeCA [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeCA [ 0 ] , lCenterEdgeCA [ 1 ] , lCenterEdgeCA [ 2 ] ) ;
			    		if (normBinding == PER_VERTEX)
			    			glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
			    		break;
	  	    	case 1:
			    		if (normBinding == PER_VERTEX)
		    			  glNormal3f ( lNormalEdgeAB [ 0 ] , lNormalEdgeAB [ 1 ] , lNormalEdgeAB [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeAB [ 0 ] , lCenterEdgeAB [ 1 ] , lCenterEdgeAB [ 2 ] ) ;
			    		if (normBinding == PER_VERTEX)
			    			glNormal3f ( lNormalEdgeBC [ 0 ] , lNormalEdgeBC [ 1 ] , lNormalEdgeBC [ 2 ] ) ;
				    	if (lFullPrevention )
		    	  		reapplyMaterial( v , frontMaterial , backMaterial , frontMatBind , backMatBind , frontAndBack ) ;
			    		glVertex3f ( lCenterEdgeBC [ 0 ] , lCenterEdgeBC [ 1 ] , lCenterEdgeBC [ 2 ] ) ;
			    		break;
	  	    }
				}
		  }
		}
      
    if (!fixed) 
		{
		  glEnd();
		  glPopName();
		}
  }
  
  if (fixed) 
  {
  	glEnd();
  }
  
	if ( lHighlightState == 1 )
	{
		glStencilFunc ( GL_ALWAYS, 0, 1 ) ;
	}

  /** glEndList(); **/
  
  if (freeFrontMatBind) free(frontMatBind);
  if (freeBackMatBind) free(backMatBind);

  errcode = glGetError();
  if (errcode!=GL_NO_ERROR)
    printf("%s by glDrawIndexedGeom\n", gluErrorString( errcode ) );
  
  /** return dpl; **/
  return 1;
}


GLuint glDrawSphereSet(int oneSphDSPL, float * coordinates, int lencoord, 
		       float *frontMaterial[5], int lenfm[5],
		       float *backMaterial[5],  int lenbm[5],
		       int frontMatBind[5], int backMatBind[5],
		       int frontAndBack, int noLightCol,
		       int fillMode, int slices, int stacks,
		       int * highlight, int lenhighlight)
    
{
  int freeFrontMatBind = 0, freeBackMatBind=0;
  /** GLuint dpl; **/
  int i, j, face,
      NONE = -1, OVERALL = 10, PER_VERTEX = 11, PER_PART = 12,
      propConst[] = { GL_AMBIENT, GL_DIFFUSE, GL_EMISSION, GL_SPECULAR,
		      GL_SHININESS };
   int i4;
   int ii ;
   int lHighlightState ;

   GLenum errcode;
  /* check front material binding parameter */
  if (frontMaterial)
    {
      if (!frontMatBind)
	{
	  frontMatBind = (int *)malloc(5*sizeof(int));
	  freeFrontMatBind = 1; 
	  for (i=0; i<5; i++) 
	    {
	      if (lenfm[i] == lencoord) 
		frontMatBind[i]=PER_PART;
	      else if (lenfm[i] == 1) frontMatBind[i]=OVERALL;
	    }
	}
    }

  /* check back material binding parameter */
  if (backMaterial)
    {
      if (!backMatBind)
	{
	  backMatBind = (int *)malloc(5*sizeof(int));
	  freeBackMatBind = 1;
	  for (i=0; i<5; i++) 
	    {
	      if (lenbm[i] == lencoord)
		backMatBind[i]=PER_PART;
	      else if (lenbm[i] == 1) backMatBind[i]=OVERALL;
	    }
	}
    }

  /** dpl = glGenLists(1); **/
  /** glNewList(dpl, GL_COMPILE_AND_EXECUTE); **/

  if (!frontAndBack)
    face = GL_FRONT;
  else
    face = GL_FRONT_AND_BACK;
  
//  if (fillMode == GL_LINES)
//    {
//      glDisable(GL_LIGHTING);
//      /*    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
//    } 
//  else
//    glEnable(GL_LIGHTING);

  /* initialize color memory */
  isNewColor(NULL,1);
  isNewMaterial(0,0,NULL,1);
  
	if ( lenhighlight > 0 )
	{
		glStencilFunc ( GL_ALWAYS, 0, 1 ) ;
	}
  lHighlightState = 0 ;

  /* loop over spheres */
  
	for (i=0; i<lencoord; i++) 
    {
		/* set PER_PART (e.g. per sphere) color/material properties */
      
		i4 = i * 4 ;
		if (frontMaterial) 
		{
			if (frontMatBind[noLightCol] == PER_PART)
			{
				if ( isNewColor(&frontMaterial[noLightCol][i4],1) ) 
				{
					glColor4fv( &frontMaterial[noLightCol][i4] );
				}
			}
		}
      
		if ( (fillMode != GL_LINES) && (fillMode != GL_POINTS) )
		{
			if (frontMaterial) 
			{
				for (j=0; j<5; j++) 
				{
					if (frontMatBind[j] == PER_PART) 
					{
						if (j == 4) ii = i;
						else ii = i4; 
						if ( isNewMaterial( face, j, &frontMaterial[j][ii],1) ) 
						{
							glMaterialfv( (GLenum)face, (GLenum)propConst[j], 
										  &frontMaterial[j][ii] );
						}
					}
				}
			}
			if (backMaterial && !frontAndBack)
			{
				for (j=0; j<5; j++) 
				{
					if (backMatBind[noLightCol] == PER_PART) 
					{
						if (j == 4) ii = i;
						else ii = i4; 
						if ( isNewMaterial( GL_BACK, j, &backMaterial[j][ii],1) )
						{
							glMaterialfv( GL_BACK, (GLenum)propConst[j], &backMaterial[j][ii] );
						}
					}
				}
			}
		}
      
		glPushName(i);
		glPushMatrix();
		glTranslatef(coordinates[i4], coordinates[i4+1], coordinates[i4+2]);
		glScalef(coordinates[i4+3], coordinates[i4+3], coordinates[i4+3]);

		if ( lenhighlight > 0 )
		{
			if ( lHighlightState == 0 )
			{
				if ( highlight [ i ] != 0 )
				{
	        glStencilFunc(GL_ALWAYS, 1, 1) ;
					lHighlightState = 1 ;
				}
			}
			else
			{
				if ( highlight [ i ] == 0 )
				{
	        glStencilFunc(GL_ALWAYS, 0, 1) ;
					lHighlightState = 0 ;
				}
			}
		}

		glCallList(oneSphDSPL);
		/*    extractedGlutSolidSphere(coordinates[i*4+3], slices, stacks); */
		glPopMatrix();
		glPopName();
	}

	if ( lHighlightState == 1 )
	{
		glStencilFunc ( GL_ALWAYS, 0, 1 ) ;
	}

	/** glEndList(); **/
	if (freeFrontMatBind) free(frontMatBind);
	if (freeBackMatBind) free(backMatBind);

	
	errcode = glGetError();
	if (errcode!=GL_NO_ERROR)
	{
		printf("%s by glDrawSphereSet\n", gluErrorString( errcode ) );
		return 0;
	}

	/** return  dpl; **/
	return 1;
}

/****************************************************************
  TRACKBALL Object
****************************************************************/

/*
 * Local function for the trackball
 */

static void track_vcopy(const float *v1, float *v2)
{
    register int i;
    for (i = 0 ; i < 3 ; i++)
        v2[i] = v1[i];
}

static void track_vcross(const float *v1, const float *v2, float *cross)
{
    float temp[3];

    temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
    temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
    temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
    track_vcopy(temp, cross);
}

static float track_vlength(const float *v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static void track_vscale(float *v, float div)
{
    v[0] *= div;
    v[1] *= div;
    v[2] *= div;
}

static void track_vnormal(float *v)
{
    track_vscale(v,1.0/track_vlength(v));
}

/*
 *  Given an axis and angle, compute quaternion.
 */
static void track_axis_to_quat(float a[3], float phi, float q[4])
{
  track_vnormal(a);
  track_vcopy(a,q);
  track_vscale(q,sin(phi/2.0));
  q[3] = cos(phi/2.0);
}

/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
static float track_project_to_sphere(float r, float x, float y)
{
  float d, t, z;

  d = sqrt(x*x + y*y);
  if (d < r * 0.70710678118654752440) {    /* Inside sphere */
    z = sqrt(r*r - d*d);
  } else {           /* On hyperbola */
    t = r / 1.41421356237309504880;
    z = t*t / d;
  }
  return z;
}

/*
 * Simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * p1x, p1y: last cursor position  (assumed in range -1.0 ... 1.0)
 * p2x, p2y: current cursor position  (assumed in range -1.0 ... 1.0)
 * 
 * result: q, quaternion describing the rotation
 */
static void trackball(float q[4], float p1x, float p1y, float p2x, float p2y,
		      float size)
{
  int i;
  float a[3]; /* Axis of rotation */
  float phi;  /* how much to rotate about axis */
  float p1[3], p2[3], d[3];
  float t;

  if (p1x == p2x && p1y == p2y) { /* Zero rotation */
    q[0] = q[1] = q[2] = 0.0; q[3] = 1.0;
    return;
  }

  /*
   * First, figure out z-coordinates for projection of P1 and P2 to
   * deformed sphere
   */
  p1[0] = p1x;
  p1[1] = p1y;
  p1[2] = track_project_to_sphere(size,p1x,p1y);
  p2[0] = p2x;
  p2[1] = p2y;
  p2[2] = track_project_to_sphere(size,p2x,p2y);

  /*
   *  Now, we want the cross product of P1 and P2
   */
  track_vcross(p2,p1,a);

  /*
   *  Figure out how much to rotate around that axis.
   */
  for (i=0; i<3; i++) d[i] = p1[i] - p2[i];

  t = track_vlength(d) / (2.0*size);

  /*
   * Avoid problems with out-of-control values...
   */
  if (t > 1.0) t = 1.0;
  if (t < -1.0) t = -1.0;
  phi = 2.0 * asin(t);

  track_axis_to_quat(a,phi,q);
}

/*
 * Build a rotation matrix, given a quaternion rotation.
 *
 */
static void track_build_rotmatrix(float m[4][4], float q[4])
{
  m[0][0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
  m[0][1] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
  m[0][2] = 2.0 * (q[2] * q[0] + q[1] * q[3]);
  m[0][3] = 0.0;

  m[1][0] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
  m[1][1]= 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
  m[1][2] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
  m[1][3] = 0.0;

  m[2][0] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
  m[2][1] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
  m[2][2] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);
  m[2][3] = 0.0;

  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 0.0;
  m[3][3] = 1.0;
}

/*****************************************************************/

#define Py_Try(BOOLEAN) {if(!(BOOLEAN)) return NULL;}

#define PyObjtrackball_Check(op) ((op)->ob_type == &PyObjtrackball_type)

typedef struct {
  PyObject_HEAD
    float trackballsize;
    float scale;
    float quat[4];
    float matrix[4][4];
    int renormcount;
} PyObjtrackball;

/* staticforward PyTypeObject PyObjtrackball_type; */


static PyObject *Pytrackball(PyObject *self, PyObject *args)
{
  float p1x, p1y, p2x, p2y;
  int width, height, mat=0;
  PyObjtrackball *t = (PyObjtrackball *)self;

  if(!PyArg_ParseTuple(args, "ffffii|i", &p1x, &p1y, &p2x, &p2y,
		       &width, &height, &mat ))
    return NULL;

  trackball(t->quat,
	    (t->scale*p1x - width) / width,  /* assumed to be -1.0 .... 1.0 */
	    (height - t->scale*p1y) / height,
	    (t->scale*p2x - width) / width,
	    (height - t->scale*p2y) / height,
	    t->trackballsize);

  if (mat) track_build_rotmatrix(t->matrix, t->quat);

  Py_INCREF(Py_None);
  return Py_None;
}


static struct PyMethodDef PyObjtrackball_methods[] = {
    {"update", Pytrackball, METH_VARARGS},
    {NULL, NULL}
};


static void PyObjtrackball_dealloc(PyObjtrackball *self)
{
  PyObject_Del(self);
}

/*
   return an Numeric 1D array of 'len' floats
*/
static PyArrayObject *track_array_vector_float(float *data, int len)
{
  PyArrayObject *vector;

  vector = (PyArrayObject *)PyArray_FromDims(1, &len, PyArray_FLOAT);
  if (!vector)
    {
      PyErr_SetString(PyExc_RuntimeError,
		      "Failed to allocate memory for vector");
      return NULL;
    }
  memcpy(vector->data, data, len*sizeof(float));
  return vector;
}


static PyObject *PyObjtrackball_getattr(PyObjtrackball *self, PyObject *nameobj)
{
  const char *name = PyUnicode_AsUTF8(nameobj);
  if (name == NULL) return NULL;
  if(strcmp(name, "size") == 0)
    return Py_BuildValue("f", self->trackballsize);
  if(strcmp(name, "scale") == 0)
    return Py_BuildValue("f", self->scale);
  else if (strcmp(name, "quat") == 0)
      return (PyObject *)(track_array_vector_float(self->quat, 4));
  else if (strcmp(name, "mat") == 0)
      return (PyObject *)(track_array_vector_float((float *)(self->matrix), 16));
  else if (strcmp(name, "renorm") == 0)
      return Py_BuildValue("i", self->renormcount);

  return PyObject_GenericGetAttr((PyObject *)self, nameobj);
}

static int PyObjtrackball_setattr(PyObjtrackball *self, PyObject *nameobj, PyObject *v)
{
  const char *name = PyUnicode_AsUTF8(nameobj);
  if (name == NULL) return -1;
  if (v == NULL) {
    PyErr_SetString(PyExc_AttributeError, "cannot delete trackball attributes");
    return -1;
  }
  if(strcmp(name, "size") == 0)
    {
      Py_Try(PyArg_Parse(v, "f", &self->trackballsize));
      return 0;
    }
  else if (strcmp(name, "scale") == 0)
    {
      Py_Try(PyArg_Parse(v, "f", &self->scale));
      return 0;
    }
  else if (strcmp(name, "renom") == 0)
    {
      Py_Try(PyArg_Parse(v, "i", &self->renormcount));
      return 0;
    }

  PyErr_SetString(PyExc_ValueError, "Sorry, bad or ReadOnly data member");
  return -1;
}

static PyObject *PyObjtrackball_repr(PyObjtrackball *self)
{
  return PyUnicode_FromString("Trackball Object");
}


static PyTypeObject PyObjtrackball_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "trackball",
    .tp_basicsize = sizeof(PyObjtrackball),
    .tp_dealloc = (destructor)PyObjtrackball_dealloc,
    .tp_getattro = (getattrofunc)PyObjtrackball_getattr,
    .tp_setattro = (setattrofunc)PyObjtrackball_setattr,
    .tp_repr = (reprfunc)PyObjtrackball_repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = PyObjtrackball_methods,
};





static PyObjtrackball *Newtrackball(float size, float scale, int renorm)
{
  PyObjtrackball *self;
  int i,j;

  if (PyType_Ready(&PyObjtrackball_type) < 0) return NULL;
  Py_Try((self = PyObject_New(PyObjtrackball, &PyObjtrackball_type)));

/******Initialize your structure values here******/
  self->trackballsize = size;
  self->scale = scale;
  self->renormcount = renorm;
  for (i=0; i<4; i++) {
    self->quat[i] = 0.0;
    for (j=0; j<4; j++) {
      self->matrix[i][j] = 0.0;
    }
    self->matrix[i][i] = 1.0;
  }
/*************************************************/

  return self;
}


/*  static PyObject *Create_trackball(PyObject *self, PyObject *args, PyObject *kw) */
static PyObject *Create_trackball(PyObject *self, PyObject *args)
{
  /*static char * argnames[] = { "size", "scale", "renorm", NULL }; */
  PyObjtrackball *result;
  int renorm=97;
  float size=0.8, scale=2.0;

  /*if(!PyArg_ParseTupleAndKeywords(args, kw, "|ffi", argnames,
    &size, &scale, &renorm))*/
  if(!PyArg_ParseTuple(args,  "|ffi", &size, &scale, &renorm))
    return NULL;

  result = Newtrackball(size, scale, renorm);

  if(!result)
    {
      PyErr_SetString(PyExc_RuntimeError, "Failed to allocate memory");
      return NULL;
    }

  return (PyObject *)result;
}



static PyObject* gl_NamedPoints1(PyObject* self, PyObject* args)
{
    int i, size, name;
    PyObject *vop;
    PyArrayObject *mp;  
    float *data;
    if(! (PyArg_ParseTuple(args, "O", &vop))) return NULL;

    mp = (PyArrayObject *)PyArray_ContiguousFromObject(
						vop, PyArray_FLOAT, 0, 10);
    if ( !mp ) return NULL;

    size = PyArray_Size((PyObject *)mp);
    if ((size%3) != 0){
	PyErr_SetString(PyExc_ValueError, "matrix length sould be divisible by 3");
	return NULL;
    }
    printf("in gl_NamedPoints %d\n", mp->nd);
    for (data = (float *)mp->data, name=i=0; i<(size/3); i++, name++, data+=3)
	{
	  printf("picking %d %p\n", i, data);
	  printf("picking %f %f %f\n", data[3*i], data[3*i+1], data[3*i+2]);
	  glPushName(name);
	  glBegin(GL_POINTS);
	  /* glVertex3fv(data); */
	  glEnd();
	  glPopName();
	}
    Py_INCREF(Py_None);
    return Py_None; 
}

void namedPoints(int nb, const float *coords)
{
  int i, name;

  for (name=i=0; i<nb; i++, name++)
    {
      /* printf("picking %f %f %f\n", coords[3*i], coords[(3*i)+1], 
	 coords[(3*i)+2]); */
      glPushName(name);
      glBegin(GL_POINTS);
      glVertex3fv(&coords[3*i]);
      glEnd();
      glPopName();
    }  
}

void *map_lookup(char *mp[][2], char *nm) {
  int i=0;
  while (mp[i][0] != NULL) {
    if (strcmp(mp[i][0], nm) == 0) {
      return (void *)mp[i][1];
    }
    i = i+1;
  }
  return NULL;
}

/*
 * char *glutFonts[][2] = {
  {"glut9by15", (char *)GLUT_BITMAP_9_BY_15},
  {"glut8by13", (char *)GLUT_BITMAP_8_BY_13},
  {"glutTimesRoman10", (char *)GLUT_BITMAP_TIMES_ROMAN_10},
  {"glutTimesRoman24", (char *)GLUT_BITMAP_TIMES_ROMAN_24},
#if (GLUT_API_VERSION >= 3)
  {"glutHelvetica10", (char *)GLUT_BITMAP_HELVETICA_10},
  {"glutHelvetica12", (char *)GLUT_BITMAP_HELVETICA_12},
  {"glutHelvetica18", (char *)GLUT_BITMAP_HELVETICA_18},
  {"glutStrokeRoman", (char *)GLUT_STROKE_ROMAN},
  {"glutStrokeRomanFixed", (char *)GLUT_STROKE_MONO_ROMAN},
#endif
  {NULL, NULL},
};

void bitmapString(char *fontname, char *string)
{
  int len, i;
  void *font;

  font = map_lookup(glutFonts, fontname);
  if (font) {
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
      glutBitmapCharacter(font, string[i]);
    }
  }
}
*/

%}

%typemap(argout) double OUT_ARRAY2D[ANY][ANY]
%{
   $result = l_output_helper2($result, (PyObject *)array$argnum);
%}
%typemap(in, numinputs=0) double OUT_ARRAY2D[ANY][ANY] (PyArrayObject *array, 
							int out_dims[2])
%{
  out_dims[0] = $1_dim0;
  out_dims[1] = $1_dim1;
  array = (PyArrayObject *)PyArray_FromDims(2, out_dims, PyArray_DOUBLE);

  // so we'll free this memory when this
  // array will be garbage collected
  MGL_SET_OWNDATA(array); 


  $1 = (double (*)[$1_dim1])array->data;
%}


%typemap(in) double VECTOR[ANY] (PyArrayObject *array, int expected_dims[1])
%{
  expected_dims[0] = $1_dim0;
  if (expected_dims[0]==1) expected_dims[0]=0;
  array = contiguous_typed_array($input, PyArray_DOUBLE, 1, expected_dims);
  if (! array) return NULL;
  $1 = (double *)array->data;
%}

%typemap(freearg) double VECTOR[ANY]

%{
  if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
   
%}

%apply double VECTOR[ANY] {double mat_data[16]}
%apply double OUT_ARRAY2D[ANY][ANY] {double a[4][4]}
void glCleanRotMat(double mat_data[16], double a[4][4]);
void extractedGlutSolidSphere ( GLdouble radius , GLint slices , GLint stacks , 
                                int insideout = 0 ) ;
void solidCylinder ( GLdouble radiusBase ,
					 GLdouble radiusTop ,
                     GLdouble height ,
                     GLint slices ,
                     GLint stacks = 1 ,
                     int insideout = 0 ) ;

%typemap(in) const float *coords
{
  long buffer_len;
  if (PyObject_AsWriteBuffer( $input, (void**)&$1, &buffer_len))
    return NULL;
  if (! $1) return PyErr_Format( PyExc_ValueError,
				      "NULL buffer not accepted");
}

void namedPoints(int nb, const float *coords);

/** TYPEMAPS for glDrawSphereSet **/
%typemap(in) int getMatBind[5] (PyArrayObject *mb_array = NULL, 
				      int expected_bind_dims)
%{
  expected_bind_dims = 5;
  if ($input == Py_None){
    mb_array = NULL;
    $1 = NULL;
  }
  else
    {
      mb_array = contiguous_typed_array($input, PyArray_INT, 1,
					&expected_bind_dims); 
      if (!mb_array)
	{
	  /* Py_DECREF((PyObject *)c_array); */
	  return NULL;
	}
      $1 = (int *)mb_array->data;
    }
%}

%typemap(freearg)int getMatBind[5]
%{
  if (mb_array$argnum && $1)
    Py_DECREF(mb_array$argnum);
%}


%typemap(in) (float * coordinates, int lencoord)(PyArrayObject * c_array,
				       int expected_coord_dims[2])
%{
 expected_coord_dims[0] = -1;
 expected_coord_dims[1] = 4;
 c_array = contiguous_typed_array($input, PyArray_FLOAT, 2, 
				   expected_coord_dims);
 if (!c_array) return NULL;
 $1 = (float *)c_array->data;
 $2 = c_array->dimensions[0];
%}

%typemap(freearg)(float * coordinates, int lencoord)
%{
  if (c_array$argnum)
    Py_DECREF(c_array$argnum);
%}


%typemap(in) (float *getMaterial[5], int lenm[5])(PyArrayObject * m_array[5],
                                                   int expected_prop_dims[2],
                                                   int intdims[5],
                                                   float * lala[5])
%{
  expected_prop_dims[0] = -1;
  expected_prop_dims[1] = 4;
  if ($input  == Py_None) 
	{
      $1 = NULL;
      $2 = NULL;
	}
  else
  {
    int nd, i, j;
    PyObject * lstitem;
    for (i=0; i<5; i++)
    {
		  nd = (i==4) ? 1:2;
	    lstitem = PyList_GetItem($input, i);
			m_array[i] = contiguous_typed_array(lstitem, PyArray_FLOAT, nd,
					     expected_prop_dims);
			if (!m_array[i]) 
	  	{
	      for (j=0; j<i; j++)
	      {
	        if (m_array[j]) 
	        {
	        	Py_DECREF((PyObject *)m_array[j]);
	        }
	      }
	      return NULL;
	    }
	    
	    lala[i] = (float *)m_array[i]->data;
	    intdims[i] = m_array[i] -> dimensions[0]; 
    }
    $1 = lala;
    $2 = intdims;
  } 
%}

%typemap(freearg) (float *getMaterial[5], int lenm[5])
%{
  if ($1)
  {
  	int i;
    for (i = 0; i < 5; i++)
	  {
	    if (m_array$argnum[i])
	      Py_DECREF((PyObject *)m_array$argnum[i]);
	  }
  }
%}

%typemap(in) (int * highlight, int lenhighlight)(PyArrayObject * c_array=NULL)
%{
	 if ($input != Py_None)
	 {
		 c_array = contiguous_typed_array($input, PyArray_INT, 1, NULL);
		 if (!c_array ) return NULL;
		 $1 = (int *)c_array->data;
		 $2 = c_array->dimensions[0];
	 }
	 else
	 {
	   c_array = NULL;
	   $1 = 0;
	   $2 = NULL;
	 }
%}

%typemap(freearg)(int * highlight, int lenhighlight)
%{
  if (c_array$argnum)
    Py_DECREF(c_array$argnum);
%}

%apply int getMatBind[5] {int frontMatBind[5]};
%apply int getMatBind[5] {int backMatBind[5]};
%apply (float *getMaterial[5], int lenm[5]) {(float *frontMaterial[5], int lenfm[5]), (float *backMaterial[5], int lenbm[5])};
GLuint glDrawSphereSet(int oneSphDSPL, float * coordinates, int lencoord,
		     float *frontMaterial[5] = NULL , int lenfm[5] = NULL, 
		     float *backMaterial[5] = NULL, int lenbm[5] = NULL,
		     int frontMatBind[5]  = NULL, 
		     int backMatBind[5] = NULL,
		     int frontAndBack = 0, int noLightCol = 1,
		     int fillMode = -1, int slices = 10, int stacks = 10,
		     int * highlight=NULL, int lenhighlight=0);

/*void bitmapString(char *fontame, char *string);*/


/* TYPEMAPS for glDrawIndexedGeom */
%typemap(in) (float * coordinates, int lenc)(PyArrayObject * c_array,
				       int expected_coord_dims[2])
%{
  if($input  == Py_None) 
    {
      c_array = NULL;
      $1 = NULL;
      $2 = 0;
    }
     else
  {
    expected_coord_dims[0] = -1;
    expected_coord_dims[1] = 3;
    c_array = contiguous_typed_array($input, PyArray_FLOAT, 2, 
				     expected_coord_dims);
    if (!c_array) return NULL;
    $1 = (float *)c_array->data;
    $2 = c_array->dimensions[0];
  }
%}

%typemap(freearg)(float *coordinates, int lenc)
%{
  if (c_array$argnum)
    Py_DECREF(c_array$argnum);
%}

%typemap(in) (float *carr, int len[2])(PyArrayObject * array = NULL, int intdims[2])

%{
  
  if($input  == Py_None) 
    {
      array = NULL;
      $1 = NULL;
      $2 = NULL;
    }
  else
    {
      array = contiguous_typed_array($input, PyArray_FLOAT, 2, NULL);
      if (!array) return NULL;
      $1 = (float *)array->data;
		  intdims[0] = array->dimensions[0];
		  intdims[1] = array->dimensions[1];
		  $2 = intdims;
    }
%}

%typemap(freearg)(float *carr, int len[2])
%{
  if (array$argnum)
    Py_DECREF(array$argnum);
%}

%typemap(in) (int *indices, int lenind[2])(PyArrayObject * array, int intdims[2])

%{
  if($input  == Py_None) 
    {
      array = NULL;
      $1 = NULL;
      $2 = NULL;
    }
  else
    {
      array = contiguous_typed_array($input, PyArray_INT, 2, NULL);
      if (!array) return NULL;
      $1 = (int *)array->data;
		  intdims[0] = array->dimensions[0];
		  intdims[1] = array->dimensions[1];
		  $2 = intdims;
    }
%}

%typemap(freearg)(int *indices, int lenind[2])
%{
  if (array$argnum)
    Py_DECREF(array$argnum);
%}


%typemap(in) (int * highlight, int lenhighlight)(PyArrayObject * c_array=NULL)
%{
	 if ($input != Py_None)
	 {
		 c_array = contiguous_typed_array($input, PyArray_INT, 1, NULL);
		 if (!c_array ) return NULL;
		 $1 = (int *)c_array->data;
		 $2 = c_array->dimensions[0];
	 }
	 else
	 {
	   c_array = NULL;
	   $1 = NULL;
	   $2 = 0;
	 }
%}

%typemap(freearg)(int * highlight, int lenhighlight)
%{
  if (c_array$argnum)
    Py_DECREF(c_array$argnum);
%}

%apply (float *carr, int len[2]) {(float *normals, int lennorm[2]),
				     (float *texIndices, int lentexind[2])};
GLuint glDrawIndexedGeom(int type, float *coordinates, int lenc,
			 int *indices, int lenind[2], 
			 float *normals=NULL, int lennorm[2]=NULL,
			 float *texIndices=NULL, int lentexind[2]=NULL,
			 float *frontMaterial[5]=NULL, int lenfm[5]=NULL,
			 float *backMaterial[5]=NULL, int lenbm[5]=NULL,
			 int frontMatBind[5]=NULL, int backMatBind[5]=NULL,
			 int frontAndBack=0,  int noLightCol=1,
			 int sharpColorBoundaries=1,
			 int preventIntelBug=0
			 ,int * highlight=NULL, int lenhighlight=0 
       );


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

%native( glTrackball)       Create_trackball;

%init %{

  import_array(); /* load the Numeric PyCObjects */

  PyDict_SetItemString(d, "_numeric", PyInt_FromLong(1L));
%}

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

def glTriangleNormals(vertices, triangles, mode = "PER_FACE" ):
    if mode == "PER_FACE":
        return triangleNormalsPerFace(vertices, triangles)
    elif mode == "PER_VERTEX":
        return triangleNormalsPerVertex(vertices, triangles)  
    elif mode == "BOTH":
        return triangleNormalsBoth(vertices, triangles) 
%}
