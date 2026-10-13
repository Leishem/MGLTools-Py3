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

#ifndef SDFLIB_H
#define SDFLIB_H

/*First, set the parameters of the SDF grid.
 * SIZE = size of the volume. has to be 16, 32, 64, 128, 512 or 1024. The number of voxels returned is 17, 33, 65, 129, 513 or 1025 cubed.
 * ISNORMALFLIP: to orient the normals of the input triangulation. Also, ensures that the normals are pointing outwards.
 * INSIDEZERO: the sign of the function is negative inside the surface and positive outside, if this parameter is true. 
*/
void setParameters(int size, bool isNormalFlip, bool insideZero, double bufarr[6]);

/*Then, call the function with the input triangulated data. The SDF values are returned
 * The size of the return array is (size+1)*(size+1)*(size+1)	
*/
float* computeSDF(int nverts, float* verts, int ntris, int* tris);


#endif
