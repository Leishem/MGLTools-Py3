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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"

void free_memory()
{
	int i, j, k;
	SDFLibrary::listnode* temp;
	SDFLibrary::listnode* currNode;

	printf("starting memory de-allocation\n");

	//1. Octree
	for (i = 0; i < SDFLibrary::size; i++)
	{
		for (j = 0; j < SDFLibrary::size; j++)
		{
			for (k = 0; k < SDFLibrary::size; k++)
			{
				currNode = SDFLibrary::sdf[i][j][k].tindex;

				while(currNode != NULL)
				{
					temp = currNode;
					currNode = currNode->next;
					free(temp);
				}
			}
			free(SDFLibrary::sdf[i][j]);
		}
		free(SDFLibrary::sdf[i]);
	}	
	free(SDFLibrary::sdf);

	free(SDFLibrary::values);

	if (SDFLibrary::vertices != NULL)
		free(SDFLibrary::vertices);

	if (SDFLibrary::surface != NULL)
		free(SDFLibrary::surface);

	if (SDFLibrary::normals != NULL)
		free(SDFLibrary::normals);

	if (SDFLibrary::distances != NULL)
		free(SDFLibrary::distances);

	if (SDFLibrary::queues != NULL)
		free(SDFLibrary::queues);

	if (SDFLibrary::bverts != NULL)
		free(SDFLibrary::bverts);

	printf("Memory de-allocated successfully! \n");
}

void setParameters(int Size, bool isNormalFlip, bool insideZero, double bufferArr[6])
{
        int i;
	//First the default values.
	init_all_vars();
	
	//Then, assign the actual input values.
	SDFLibrary::size = Size;
	SDFLibrary::flipNormals = isNormalFlip;
	SDFLibrary::insideZero = insideZero;

	for (i=0; i<6; i++)
	  SDFLibrary::buffArr[i] = bufferArr[i];	

	if ((Size!=16) && (Size!=32) &&(Size!=64) && (Size!=128) && (Size!=256) &&(Size!=512) &&(Size!=1024))
	{
		printf("size is incorrect\n");
		exit(1);
	}
}

float* computeSDF(int nverts, float* verts, int ntris, int* tris)
{
	int i, numb;
	float* sdfValues =NULL;
	float isoval;

	//Set up the volume grid
	initSDF();

	//Read in the Geometry
	readGeom(nverts, verts, ntris, tris);

	//Setup the Octree
	adjustData();

	//Compute the SDF
    compute();

	//Return the SDF
	numb = (SDFLibrary::size+1)*(SDFLibrary::size+1)*(SDFLibrary::size+1);
	sdfValues = (float*)(malloc(sizeof(float)*(numb)));
	isoval = 100.0f;

	for (i=0; i<numb; i++)
		if (SDFLibrary::insideZero)
			sdfValues[i] = SDFLibrary::values[i].value * SDFLibrary::values[i].signe;
		else
			sdfValues[i] = -1*SDFLibrary::values[i].value * SDFLibrary::values[i].signe;

	free_memory();

	return (sdfValues);
}	
