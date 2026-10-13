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

#include "common.h"

namespace SDFLibrary{

	double MAX_DIST;
	int size;
	int total_points, total_triangles, all_verts_touched;
	double minx, miny, minz, maxx, maxy, maxz;
	double TOLERANCE;
	int octree_depth;
	int flipNormals;
	int insideZero;

	SDFLibrary::triangle* surface;
	SDFLibrary::myVert* vertices;
	SDFLibrary::myPoint* normals;
	SDFLibrary::cell*** sdf;
	SDFLibrary::voxel* values;
	double* distances;
	
	char *ifname;

	bool* bverts;
	int* queues;
        double buffArr[6];
}

void init_all_vars()
{
	SDFLibrary::TOLERANCE = 1e-6;
	SDFLibrary::size =64;
	SDFLibrary::flipNormals =0;
	SDFLibrary::insideZero =1;
	
	SDFLibrary::ifname= NULL;
	SDFLibrary::surface = NULL;
	SDFLibrary::vertices = NULL;
	SDFLibrary::normals = NULL;
	SDFLibrary::distances = NULL;
	SDFLibrary::sdf = NULL;
	SDFLibrary::values = NULL;
	SDFLibrary::bverts = NULL;
	SDFLibrary::queues = NULL;

	SDFLibrary::total_points = SDFLibrary::total_triangles = SDFLibrary::all_verts_touched= 0;
}
