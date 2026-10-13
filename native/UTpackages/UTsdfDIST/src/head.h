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

#ifndef HEAD_H
#define HEAD_H

namespace SDFLibrary {

	#define MAX_TRIS_PER_VERT 100
	
	typedef struct _Pt_
	{
		double x;
		double y;
		double z;
		char isNull;
		
	}  myPoint;

	typedef struct _Vt_
	{
		double x;
		double y;
		double z;
		char isNull;

		int tris[MAX_TRIS_PER_VERT]; //not more than MAX_TRIS_PER_VERT triangles can share a vertex.
		int trisUsed; //elements used in the above array.

	}  myVert;


	typedef struct _tri_
	{
		int v1;
		int v2;
		int v3;

		int type; // default = -1; done =1.	wrong =3;
	} triangle;

	typedef struct listnodedef
	{
		int index;	//index of the triangle
		struct listnodedef* next;
	} listnode;

	typedef struct nodedef 
	{
		char useful;	//  0 - no triangles in it	; 1 - there are triangles in it
		char type;		//	0 - interior node		; 1 - leaf node, containing triangles
		long int no;
		listnode* tindex;		
	} cell;

	typedef struct 
	{
		double ox;
		double oy;
		double oz;

		double dx; 
		double dy;
		double dz;
	} ray; 

	typedef struct _voxel_
	{
		float value;
		signed char signe;  //-1 = inside,		1 = outside	
		bool processed;		// 1 = propagated distance FROM here. 0 = not
		myPoint closestV;	//the closest vertex on the surface
	}voxel;

	
}; //namespace SDFLibrary

#endif
