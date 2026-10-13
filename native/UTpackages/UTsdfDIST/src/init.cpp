/*
  Copyright 2002-2003 The University of Texas at Austin
  
	Authors: Lalit Karlapalem <ckl@ices.utexas.edu>
	Advisor: Chandrajit Bajaj <bajaj@cs.utexas.edu>

  This is a free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"

using namespace SDFLibrary;

extern void update_bounding_box( long int current_triangle, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, int cur_level );
extern void check_octree();
extern void start_fireworks();

extern int putdouble(double *flts, int n, FILE *fp);
extern int putInt(int *Ints, int n, FILE *fp);

int maxInd;
double TRANS_X, TRANS_Y, TRANS_Z, SCALE;

void build_octree()
{
	double t1, t2;

	t1 = getTime();

	for (int i =0; i<total_triangles; i++)
	{
		update_bounding_box((long)i, 0.0f, (double)size, 0.0f, (double)size, 0.0f, (double)size, 0);
		if (i%1000 == 0)
			printf("%d processed in octree\n", i);
	}

	t2 = getTime();
	printf("Octree constructed for the data in %f seconds\n", (t2-t1));
}

void process_triangle(int i)
{
	double p1x, p1y, p1z, p2x, p2y, p2z;
	double nx, ny, nz;
	double denom;
	int v1,v2,v3;

	v1 = surface[i].v1;	v2 = surface[i].v2;	v3 = surface[i].v3;

	//assume that the triangles are consistently oriented V1-V2-V3.
	p1x = vertices[v3].x - vertices[v2].x;
	p1y = vertices[v3].y - vertices[v2].y;
	p1z = vertices[v3].z - vertices[v2].z;
	p2x = vertices[v1].x - vertices[v2].x;
	p2y = vertices[v1].y - vertices[v2].y;
	p2z = vertices[v1].z - vertices[v2].z;

	nx = ( ( p1y * p2z ) - ( p1z * p2y ) );
	ny = ( ( p1z * p2x ) - ( p1x * p2z ) );
	nz = ( ( p1x * p2y ) - ( p1y * p2x ) );

	denom = (double)sqrt( nx*nx + ny*ny + nz*nz );

	nx /= denom;
	ny /= denom;
	nz /= denom;
	normals[i].x = (double)nx;	normals[i].y = (double)ny;	normals[i].z = (double)nz;

	//calculate the Distance of the current Triangle from the Origin
	distances[i] =(double) (-1* ( ( nx * vertices[v1].x ) + ( ny * vertices[v1].y ) + ( nz * vertices[v1].z ) ));
	surface[i].type = -1;
}

void reverse_ptrs()
{
	int i, flag=0;

	for (i=0; i<total_triangles; i++)
	{
		process_triangle(i);
		
		vertices[ surface[i].v1 ].tris [ vertices[ surface[i].v1 ].trisUsed++ ] = i;
		vertices[ surface[i].v2 ].tris [ vertices[ surface[i].v2 ].trisUsed++ ] = i;
		vertices[ surface[i].v3 ].tris [ vertices[ surface[i].v3 ].trisUsed++ ] = i;

		if (vertices[ surface[i].v1 ].trisUsed >= MAX_TRIS_PER_VERT) 
		{
			printf("more than %d triangles share this vertex... %d for vert=%d\n", MAX_TRIS_PER_VERT, vertices[ surface[i].v1 ].trisUsed, surface[i].v1);
			flag =1;
		}
		if (vertices[ surface[i].v2 ].trisUsed >= MAX_TRIS_PER_VERT) 
		{
			printf("more than %d triangles share this vertex... %d for vert=%d\n", MAX_TRIS_PER_VERT, vertices[ surface[i].v2 ].trisUsed, surface[i].v2);
			flag =1;
		}
		if (vertices[ surface[i].v3 ].trisUsed >= MAX_TRIS_PER_VERT) 
		{
			printf("more than %d triangles share this vertex... %d for vert=%d\n", MAX_TRIS_PER_VERT, vertices[ surface[i].v3 ].trisUsed, surface[i].v3);
			flag =1;
		}

		//If any of these above statements are printed, then please increase the MAX_TRIS_PER_VERT definition in head.h file and try.
		if (flag ==1)
		{
			printf("Please try changing the MAX_TRIS_PER_VERT variable in <head.h> file and rerun\n");
			exit(0);
		}
	}
}
/****
void adjustData()
{
	double tx, ty, tz;
	double cx, cy, cz;
	int i;
	
	//shift the entire inputS into the octreeArray grid (0 to size)
	//TRANS_X = minx-2; 
	//TRANS_Y = miny-2;
	//TRANS_Z = minz-2;
	tx = maxx - minx;	ty = maxy - miny;	tz = maxz - minz;
	cx = (maxx- minx)/2.0f + minx;	cy = (maxy- miny)/2.0f + miny;	cz = (maxz- minz)/2.0f + minz;

	if (tx>ty)
	{
		if (tx>tz)
			SCALE = tx;
		else
			SCALE = tz;
	}
	else
	{
		if (ty>tz)
			SCALE = ty;
		else
			SCALE = tz;
	}
 
	SCALE = (size - 4.0f) / SCALE ; //make sure that the re-scaled data fits into the octree grid.

	for (i = 0; i < total_points; i++) 
	{
		vertices[i].x = (((vertices[i].x - cx) * SCALE) + (size/2.0f));
		vertices[i].y = (((vertices[i].y - cy) * SCALE) + (size/2.0f)); 
		vertices[i].z = (((vertices[i].z - cz) * SCALE) + (size/2.0f));
	}

	minx = (minx - cx) * SCALE + (size/2.0f);
	miny = (miny - cy) * SCALE + (size/2.0f);
	minz = (minz - cz) * SCALE + (size/2.0f);
	maxx = (maxx - cx) * SCALE + (size/2.0f);
	maxy = (maxy - cy) * SCALE + (size/2.0f);
	maxz = (maxz - cz) * SCALE + (size/2.0f);
	
	printf("translate distances are: %f %f %f and scale factor is: %f \n", 
			TRANS_X, TRANS_Y, TRANS_Z, SCALE);
	
	printf("Moved Bounding box is: %f %f %f to %f %f %f \n", minx, miny, minz, maxx, maxy, maxz);
	
	//Then re-calculate the normals of the triangles. 
	reverse_ptrs();

	//This wud align them in a consistent manner. ie: all out or all in. :-)
	if (flipNormals)
		start_fireworks();

	//Then build the Octree.
	build_octree();
}
****/

void adjustData()
{
	double tx, ty, tz;
	double cx, cy, cz;
	int i;
	
	//shift the entire inputS into the octreeArray grid (0 to size)
	TRANS_X = buffArr[0] + buffArr[1]; 
	TRANS_Y = buffArr[2] + buffArr[3]; 
	TRANS_Z = buffArr[4] + buffArr[5]; 
	
	tx = maxx - minx;			ty = maxy - miny;			tz = maxz - minz;	
	tx = (size-TRANS_X)/tx;		ty = (size-TRANS_Y)/ty;		tz = (size-TRANS_Z)/tz;
	cx = (maxx- minx)/2.0f + minx;	cy = (maxy- miny)/2.0f + miny;	cz = (maxz- minz)/2.0f + minz;

	if (tx<ty)
	{
		if (tx<tz)
			SCALE = tx;
		else
			SCALE = tz;
	}
	else
	{
		if (ty<tz)
			SCALE = ty;
		else
			SCALE = tz;
	}
/****
	for (i = 0; i < total_points; i++) 
	{
		vertices[i].x = ((vertices[i].x - cx) * SCALE) + (size/2.0f);
		vertices[i].y = ((vertices[i].y - cy) * SCALE) + (size/2.0f); 
		vertices[i].z = ((vertices[i].z - cz) * SCALE) + (size/2.0f);
	}

	minx = (minx - cx) * SCALE + (size/2.0f);
	miny = (miny - cy) * SCALE + (size/2.0f);
	minz = (minz - cz) * SCALE + (size/2.0f);
	maxx = (maxx - cx) * SCALE + (size/2.0f);
	maxy = (maxy - cy) * SCALE + (size/2.0f);
	maxz = (maxz - cz) * SCALE + (size/2.0f);
*****/
	for (i = 0; i < total_points; i++) 
 	{
 		vertices[i].x = ((vertices[i].x - cx) * tx) + (size/2.0f);
 		vertices[i].y = ((vertices[i].y - cy) * ty) + (size/2.0f); 
 		vertices[i].z = ((vertices[i].z - cz) * tz) + (size/2.0f);
 	}

	minx = (minx - cx) * tx + (size/2.0f);
	miny = (miny - cy) * ty + (size/2.0f);
	minz = (minz - cz) * tx + (size/2.0f);
	maxx = (maxx - cx) * ty + (size/2.0f);
	maxy = (maxy - cy) * tx + (size/2.0f);
	maxz = (maxz - cz) * ty + (size/2.0f);

	//printf("translate distances are: %f %f %f and scale factor is: %f \n", 
	//       TRANS_X, TRANS_Y, TRANS_Z, SCALE);
	
	printf("Moved Bounding box is: %f %f %f to %f %f %f \n", minx, miny, minz, maxx, maxy, maxz);
	//Then re-calculate the normals of the triangles. 
	reverse_ptrs();

	//This wud align them in a consistent manner. ie: all out or all in. :-)
	if (flipNormals)
		start_fireworks();

	//Then build the Octree.
	build_octree();
}


void setOctree_depth()
{
	switch (size) {
	case (16):
		octree_depth = 4;
	break;
	case (32):
		octree_depth = 5;
	break;
	case (64):
		octree_depth = 6;
	break;
	case (128):
		octree_depth = 7;
	break;
	case (256):
		octree_depth = 8;
	break;
	case (512):
		octree_depth = 9;
	break;
	case (1024):
		octree_depth = 9;
	break;

	default:
		printf("This version can only deal with Volumes of sizes 16, 32, 64, 128, 256, 512 or 1024\n");
		exit(0);
	}
}

void initSDF()
{
    int i, j, k;
	
	MAX_DIST =(double) (size * sqrt(3.0));
	minx = miny = minz = 10000.0;
	maxx = maxy = maxz = -10000.0;
	
	maxInd =-1;
		
	setOctree_depth();

	sdf = (cell***) malloc( sizeof(int) * (size));
    for (i = 0; i < size; i++)
	{
		sdf[i] = (cell**) malloc( sizeof(int) * (size));
		for (j = 0; j < size; j++)
		{
			sdf[i][j] = (cell*) malloc( sizeof(cell) * (size));
			for (k = 0; k < size; k++)
			{
				sdf[i][j][k].useful = 0;
				sdf[i][j][k].type = 1;
				sdf[i][j][k].no = 0;
				sdf[i][j][k].tindex = NULL;					
			}
		}
	}

	k = (size+1)*(size+1)*(size+1);
	values = (voxel*)(malloc(sizeof(voxel) * k));
	bverts = (bool*)(malloc(sizeof(bool)*k));
	queues = (int*)(malloc(sizeof(int)*k));

	for (i=0; i<k; i++)
	{
		values[i].value = (float)MAX_DIST;
		values[i].signe = 0;
		values[i].processed =0;
		values[i].closestV.x = MAX_DIST;
		values[i].closestV.y = MAX_DIST;
		values[i].closestV.z = MAX_DIST;
		bverts[i] = 0;
	}
}

void check_bounds(int i)
{
	if (vertices[i].x < minx) minx = (double) vertices[i].x;
	if (vertices[i].y < miny) miny = (double) vertices[i].y;
	if (vertices[i].z < minz) minz = (double) vertices[i].z;

	if (vertices[i].x > maxx) maxx = (double) vertices[i].x;
	if (vertices[i].y > maxy) maxy = (double) vertices[i].y;
	if (vertices[i].z > maxz) maxz = (double) vertices[i].z;
}

void readGeom(int nverts, float* verts, int ntris, int* tris)
{
    int	i;

	total_points = nverts;
	total_triangles = ntris;

	printf("vert= %d and tri = %d \n", total_points,total_triangles);

	vertices = (myVert*) malloc (sizeof (myVert) * total_points);
	surface = (triangle*) malloc (sizeof (triangle) * total_triangles);
	normals = (myPoint*) malloc (sizeof (myPoint) * total_triangles);
	distances = (double*) malloc (sizeof (double) * total_triangles);

	for (i=0; i<total_points; i++)
	{
		vertices[i].x = verts[3*i+0];	vertices[i].y = verts[3*i+1];	vertices[i].z = verts[3*i+2];
		check_bounds(i);
		vertices[i].isNull = 0;		vertices[i].trisUsed =0;
		
		if (!(i% 5000))
			printf("still working on points !!!! %d \n",i);
	}
	
	printf("Finished reading the Vertices.. Now reading the Triangles\n");

	for (i=0; i<total_triangles; i++)
	{
		surface[i].v1 = tris[3*i+0];	surface[i].v2 = tris[3*i+1];	surface[i].v3 = tris[3*i+2];

		if (maxInd < surface[i].v1) maxInd = surface[i].v1;
		if (maxInd < surface[i].v2) maxInd = surface[i].v2;
		if (maxInd < surface[i].v3) maxInd = surface[i].v3;

		if (!(i% 5000))
			printf("still working on Triangles !!!! %d \n",i);
	}
	
	printf("Bounding box is: %f %f %f to %f %f %f \n", minx, miny, minz, maxx, maxy, maxz);
}
