%module sdflib 
#include <stdio.h>
#include <math.h>

%{
struct sdfgrid {
	int size[3];
	float gridSpacing;
	float origin[3];
	float corner[3];
	float *gridpoint;
};

/* Read SDF (signed distance field) grid and return the grid object */	
struct sdfgrid *
readSDF (char *sdfgridfile)
{
  FILE *fp;
  int i, j, k, n;
  int size[3];
  float gridSpacing, origin[3], corner[3];
  float *gridpoint; 	/* store signed distance field */
  struct sdfgrid *sdfgrid;

  /* open grid file */
  fp = fopen(sdfgridfile, "r"); 

  /* read the first line */
  fscanf(fp, "%d %d %d   %f   %f %f %f   %f %f %f\n", &size[0], &size[1], &size[2], &gridSpacing, &origin[0], &origin[1], &origin[2], &corner[0], &corner[1], &corner[2]);

  /* printf("%5d %5d %5d   %8.3f   %8.3f %8.3f %8.3f   %8.3f %8.3f %8.3f\n", size[0], size[1], size[2], gridSpacing, origin[0], origin[1], origin[2], corner[0], corner[1], corner[2]); */

  /* allocate space for the grid points */
  gridpoint = (float *) calloc( size[0]*size[1]*size[2], sizeof(float) );

  /* read the rest lines (grid points) */
  n = 0;
  for ( i=0; i<size[0]; i++ )
  {
      for ( j=0; j<size[1]; j++)
      {
	  for ( k=0; k<size[2]; k++ )
	  {
	      fscanf(fp, "%f\n", (gridpoint+n)); /* gets(garbage); */
	      /* printf("n = %8d, dist = %8.3f\n", n, *(gridpoint+n)); */
	      n ++;
	  }
      }
  }

  /* close grid file*/
  fclose(fp);

  /* assign read sdf grid to the object */  
  sdfgrid = (struct sdfgrid *) malloc( sizeof( struct sdfgrid ) );	/* allocate memorry before assignment */
  sdfgrid->size[0] = size[0];
  sdfgrid->size[1] = size[1];
  sdfgrid->size[2] = size[2];
  sdfgrid->gridSpacing = gridSpacing;
  sdfgrid->origin[0] = origin[0];
  sdfgrid->origin[1] = origin[1];
  sdfgrid->origin[2] = origin[2];
  sdfgrid->corner[0] = corner[0];
  sdfgrid->corner[1] = corner[1];
  sdfgrid->corner[2] = corner[2];
  sdfgrid->gridpoint = gridpoint;
  /* printf("sdfgrid works!: gridSpacing = %8.3f\n", sdfgrid->gridSpacing); */
  
  return sdfgrid;
}


/* Look up transformed coordinates tcoord in a SDF grid, tcoordLen is the length of tcoord, and
 * largestOverlapAllowed is the largest overlap allowed in the search. The subroutine returns
 * 0 if no overlap larger than largestOverlapAllowed or 1 if there is. Last update: 1/19/2006 QZ  */
int lookup (struct sdfgrid *sdfgrid, float tcoord[][3], int tcoordLen, float largestOverlapAllowed)
{
    float x, y, z, xo, yo, zo, xm, ym, zm, gs;
    int sx, sy, sz, xi, yi, zi;
    static float outside = 9999.0;
    static int OK = 0, violation = 1;
    int i;
    float dist;

  for ( i = 0; i < tcoordLen; i++ )
  {
    x = tcoord[i][0];
    y = tcoord[i][1];
    z = tcoord[i][2]; 
    /* printf("point %d = %8.3f %8.3f %8.3f\n", i, x, y, z); */

    sx = sdfgrid->size[0];
    sy = sdfgrid->size[1];
    sz = sdfgrid->size[2];
    gs = sdfgrid->gridSpacing;
    xo = sdfgrid->origin[0];
    yo = sdfgrid->origin[1];
    zo = sdfgrid->origin[2];
    xm = sdfgrid->corner[0];
    ym = sdfgrid->corner[1];
    zm = sdfgrid->corner[2];

    /* printf("%5d %5d %5d   %8.3f   %8.3f %8.3f %8.3f   %8.3f %8.3f %8.3f\n", sx, sy, sz, gs, xo, yo, zo, xm, ym, zm); */
    
    if ( x<xo || x>xm )  
	continue;
    if ( y<yo || y>ym )  
	continue;
    if ( z<zo || z>zm )  
	continue;
    xi = (int) (rintf((x-xo)/gs));	/* do not use nearbyintf, which causes segmentation fault */
    yi = (int) (rintf((y-yo)/gs));
    zi = (int) (rintf((z-zo)/gs));
    /* printf("[%8.3f, %8.3f, %8.3f] -> [%5d %5d %5d]  ", x, y, z, xi, yi, zi); */
    dist = sdfgrid->gridpoint[xi*sy*sz + yi*sz + zi];
    /* printf("dist = %8.3f\n", dist); */
    if ( dist < -largestOverlapAllowed )
    {
	return violation;
    }
  }
  return OK;
}


%}

%include numarr.i

%typemap(in) ( float tcoord[1][3], int tcoordLen) (PyArrayObject *array,
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
    $2 = ((PyArrayObject *)(array))->dimensions[0];
  }
  else
  {
   array = NULL;
   $1 = NULL;
   $2 = 0;
  }

%}

%typemap(freearg)( float tcoord[1][3], int tcoordLen)
%{
   if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}

struct sdfgrid * readSDF (char *sdfgridfile);
int lookup (struct sdfgrid *sdfgrid, float tcoord[1][3], int tcoordLen, float largestOverlapAllowed);
