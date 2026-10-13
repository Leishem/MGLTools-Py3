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

#ifndef COMMON_H
#define COMMON_H

#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif //_MSC_VER


#include <stdio.h>
#include <stdlib.h>
#include <map>

#include "head.h"



void initSDF();

void readGeom(int nverts, float* verts, int ntris, int* tris);

void adjustData();

void compute();



int isEqual (double one, double two);

int isZero(double num);

int isNegative(double num);

int isBetween(double one, double two, double num);

int isZero(SDFLibrary::myPoint one);

int isSame(SDFLibrary::myPoint one, SDFLibrary::myPoint two);

void init_all_vars();



void propagate_left(int i, int j, int k);

void propagate_bottom(int i, int j, int k);

void propagate_inside(int i, int j, int k);

void propagate_right(int i, int j, int k);

void propagate_top(int i, int j, int k);

void propagate_outside(int i, int j, int k);

void apply_distance_transform(int vi, int vj, int vk);

void insert_bound_vert(int vert);




int index2vert(int i, int j, int k);

void _vert2index(int c, int &i, int &j, int &k);

double xCoord(int i);

double yCoord(int i);

double zCoord(int i);

double getTime();


namespace SDFLibrary {

	extern double MAX_DIST;
	extern int size;

	extern triangle* surface;
	extern myVert* vertices;
	extern myPoint* normals;
	extern double* distances;
	extern cell*** sdf;
	extern voxel* values;
	extern int total_points, total_triangles, all_verts_touched;
	extern double minx, miny, minz, maxx, maxy, maxz;

	extern double TOLERANCE;

	extern int octree_depth;
	extern int flipNormals;
	extern int insideZero; 

	extern double buffArr[6];

	extern bool *bverts;
	extern int *queues;
	
}; //namespace SDFLibrary

#endif
