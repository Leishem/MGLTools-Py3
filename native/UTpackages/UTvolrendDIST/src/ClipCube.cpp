/*
  Copyright 2002-2003 The University of Texas at Austin
  
	Authors: Anthony Thane <thanea@ices.utexas.edu>
	Advisor: Chandrajit Bajaj <bajaj@cs.utexas.edu>

  This file is part of Volume Rover.

  Volume Rover is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Volume Rover is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with iotree; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// ClipCube.cpp: implementation of the ClipCube class.
//
//////////////////////////////////////////////////////////////////////

#include "ClipCube.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Lookup tables
//////////////////////////////////////////////////////////////////////

#include "LookupTables.h"

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ClipCube::ClipCube()
{
	setAspectRatio(1.0, 1.0, 1.0);
	setTextureSubCube(0,0,0,1,1,1);
}

ClipCube::ClipCube(double ratioX, double ratioY, double ratioZ, 
		double minX, double minY, double minZ, 
		double maxX, double maxY, double maxZ)
{
	setAspectRatio(ratioX, ratioY, ratioZ);
	setTextureSubCube(minX, minY, minZ, maxX, maxY, maxZ);
}

ClipCube::~ClipCube()
{
	// do nothing in the destructor
}

bool ClipCube::setAspectRatio(double ratioX, double ratioY, double ratioZ)
{
	// sets the aspect ratio of the volume render
	double maxratio;
	if (ratioX!=0.0 && ratioY!=0.0 && ratioZ!=0.0) {
		m_RatioX = ratioX;
		m_RatioY = ratioY;
		m_RatioZ = ratioZ;

		// find the maximum ratio
		maxratio = ( m_RatioX > m_RatioY ? m_RatioX : m_RatioY );
		maxratio = ( maxratio > m_RatioZ ? maxratio : m_RatioZ );

		// normalize so the max ratio is 1.0
		m_RatioX /= maxratio;
		m_RatioY /= maxratio;
		m_RatioZ /= maxratio;

		return true;
	}
	else {
		return false;
	}
}

bool ClipCube::setTextureSubCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	for (int i=0; i<8; i++) {
		m_TexCoords[i*3+0] = (TexCoords[i*3+0]<0.5?minX:maxX);
		m_TexCoords[i*3+1] = (TexCoords[i*3+1]<0.5?minY:maxY);
		m_TexCoords[i*3+2] = (TexCoords[i*3+2]<0.5?minZ:maxZ);
	}
	return true;
}

bool ClipCube::clipPlane(Polygon& result, const Plane& plane) const
{
	double distances[8];
	unsigned caseIndex;
	unsigned int numIntersections;

	// calculate the lookup table index and fill the signed distance table
	caseIndex = getCaseAndCalculateSignedDistances(distances, plane);

	// determine the number of intersections and initialize the result polygon
	numIntersections = EdgeCases[caseIndex][0];
	if (numIntersections==0) return false;

	result.setNumVerts(numIntersections);

	unsigned int c;
	double alpha;
	// for each intersection
	for (c=0; c<numIntersections; c++) {
		// get the alpha for the combination of the first vertex of the edge
		// and the second vertex of the edge
		alpha = getAlphaForEdge(distances, EdgeCases[caseIndex][c+1]);

		// interpolate the vertex coordinates and texture coordinates
		// store into result polygon
		interpVertCoords(result.getVert(c), alpha, EdgeCases[caseIndex][c+1]);
		interpTexCoords(result.getTexCoord(c), alpha, EdgeCases[caseIndex][c+1]);
	}

	return true;
}

unsigned char ClipCube::getCaseAndCalculateSignedDistances(double* distances, const Plane& plane) const
{
	unsigned int c;
	unsigned char index = 0;

	for (c=0; c<8; c++) {
		distances[c] = plane.signedDistance(
			VertCoords[c*3+0]*m_RatioX,
			VertCoords[c*3+1]*m_RatioY,
			VertCoords[c*3+2]*m_RatioZ);
		if (distances[c] > 0.0) {
			index |= 1<<c;
		}
	}
	return index;
}

double ClipCube::getAlphaForEdge(double * distances, unsigned int edgeIndex) const
{
	unsigned int v1 = Edges[edgeIndex*2+0], v2 = Edges[edgeIndex*2+1];
	double totalDistance = fabs(distances[v1]) + fabs(distances[v2]);
	return (totalDistance!=0.0 ? fabs(distances[v1]) / totalDistance : 0.0);
}

void ClipCube::interpVertCoords(double * vert, double alpha, unsigned int edgeIndex) const
{
	unsigned int v1 = Edges[edgeIndex*2+0], v2 = Edges[edgeIndex*2+1];
	vert[0] = VertCoords[v1*3+0] * m_RatioX * (1.0-alpha) + VertCoords[v2*3+0] * m_RatioX * alpha;
	vert[1] = VertCoords[v1*3+1] * m_RatioY * (1.0-alpha) + VertCoords[v2*3+1] * m_RatioY * alpha;
	vert[2] = VertCoords[v1*3+2] * m_RatioZ * (1.0-alpha) + VertCoords[v2*3+2] * m_RatioZ * alpha;
}

void ClipCube::interpTexCoords(double * texCoord, double alpha, unsigned int edgeIndex) const
{
	unsigned int v1 = Edges[edgeIndex*2+0], v2 = Edges[edgeIndex*2+1];
	texCoord[0] = m_TexCoords[v1*3+0] * (1.0-alpha) + m_TexCoords[v2*3+0] * alpha;
	texCoord[1] = m_TexCoords[v1*3+1] * (1.0-alpha) + m_TexCoords[v2*3+1] * alpha;
	texCoord[2] = m_TexCoords[v1*3+2] * (1.0-alpha) + m_TexCoords[v2*3+2] * alpha;
}




