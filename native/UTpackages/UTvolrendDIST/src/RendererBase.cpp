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

// RendererBase.cpp: implementation of the RendererBase class.
//
//////////////////////////////////////////////////////////////////////

#include "RendererBase.h"
#include "Polygon.h"
#include "ClipCube.h"
#include <math.h>

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RendererBase::RendererBase() : m_PolygonArray(100)
{
	initFlags();
	initAspectRatio();
	m_NumberOfPlanesRendered = 0;

	m_VertexArray = 0;
	m_TextureArray = 0;
	m_VertexArraySize = 0;
	m_TriangleArray = 0;
	m_TriangleArraySize = 0;
	m_NumVertices = 0;
	m_NumTriangles = 0;
}

RendererBase::~RendererBase()
{
	deallocateMemory();
}

// Initializes the renderer.  Should be called again if the renderer is
// moved to a different openGL context.  If this returns false, do not try
// to use it to do volumeRendering
bool RendererBase::initRenderer()
{
	m_BaseInitialized = true;
	setQuality(1.0);
	setNearPlane(0.0);
	setTextureSubCube(0,0,0,1,1,1);
	setDataSubVolume(0,0,0,1,1,1);
	setHintDimensions(0,0,0);
	return true;
}

// Sets the aspect ratio of the dataset.
bool RendererBase::setAspectRatio(double ratioX, double ratioY, double ratioZ)
{
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

// Specifies the portion of the uploaded texture that should be rendered.
// The extenst should range from 0 to 1.
bool RendererBase::setTextureSubCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	m_TextureSubCubeExtent.setExtents(minX, minY, minZ, maxX, maxY, maxZ);
	return true;
}

// Specifies that we are rendering a subportion of the full data.
// Used for out of core rendering.  The extents should range from 0 to 1.
bool RendererBase::setDataSubVolume(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	m_DataSubCubeExtent.setExtents(minX, minY, minZ, maxX, maxY, maxZ);
	return true;
}

// Used for out of core rendering.  The dimensions of the full dataset
bool RendererBase::setHintDimensions(unsigned int hintDimX, unsigned int hintDimY, unsigned int hintDimZ)
{
	m_HintDimX = hintDimX;
	m_HintDimY = hintDimY;
	m_HintDimZ = hintDimZ;
	return true;
}

// Quality is a number from 0 to 1.  Lower means faster.
bool RendererBase::setQuality(double quality)
{
	m_Quality = (quality>0.0?quality:0.0);
	m_Quality = (m_Quality<1.0?m_Quality:1.0);
	return true;
}

double RendererBase::getQuality() const
{
	return m_Quality;
}

// nearPlane is a number from 0 to 1.  0 means no clipping takes place.
// 1 means the entire volume is clipped.
bool RendererBase::setNearPlane(double nearPlane)
{
	m_NearPlane = (nearPlane>0.0?nearPlane:0.0);
	m_NearPlane = (m_NearPlane<1.0?m_NearPlane:1.0);
	return true;
}

double RendererBase::getNearPlane()
{
	return m_NearPlane;
}

// Returns the number of planes rendered in the last call to 
// renderVolume.
int RendererBase::getNumberOfPlanesRendered() const
{
	return m_NumberOfPlanesRendered;
}

// Allocates memory for the vertices and triangles
bool RendererBase::allocateMemory(unsigned int numVerts, unsigned int numTriangles)
{
	// only allocate new memory if the old arrays aren't already 
	// big enough
	if (numVerts>m_VertexArraySize) {
		if (!allocateVertexArray(numVerts)) {
			m_NumVertices = 0;
			m_NumTriangles = 0;
			return false;
		}
	}

	if (numTriangles>m_TriangleArraySize) {
		if (!allocateTriangleArray(numTriangles)) {
			m_NumVertices = 0;
			m_NumTriangles = 0;
			return false;
		}
	}
	m_NumVertices = numVerts;
	m_NumTriangles = numTriangles;
	return true;
}

// Deallocates the memory for vertices and triangles
void RendererBase::deallocateMemory()
{
	delete [] m_VertexArray;
	m_VertexArraySize = 0;
	delete [] m_TriangleArray;
	m_TriangleArraySize = 0;
}

// Allocate the vertex array
bool RendererBase::allocateVertexArray(unsigned int numVerts)
{
	delete [] m_VertexArray;
	delete [] m_TextureArray;
	m_VertexArray = new float[numVerts*3];
	m_TextureArray = new float[numVerts*3];
	if (m_VertexArray && m_TextureArray) {
		m_VertexArraySize = numVerts;
		return true;
	}
	else {
		m_VertexArraySize = 0;
		m_VertexArray = 0;
		m_TextureArray = 0;
		return false;
	}
}

// Allocate the triangle array
bool RendererBase::allocateTriangleArray(unsigned int numTriangles)
{
	delete [] m_TriangleArray;
	m_TriangleArray = new unsigned int[numTriangles*3];
	if (m_TriangleArray) {
		m_TriangleArraySize = numTriangles;
		return true;
	}
	else {
		m_TriangleArraySize = 0;
		return false;
	}
}

// Converts the polygon array to traingle and vertex arrays
void RendererBase::convertToTriangles()
{
	// determine the number of triangles and vertices
	unsigned int numTriangles = 0;
	unsigned int numVerts = 0;
	unsigned int c;
	for (c=0; c<m_PolygonArray.getNumPolygons(); c++) {
		numTriangles+=m_PolygonArray.getPolygon(c)->getNumTriangles();
		numVerts+=m_PolygonArray.getPolygon(c)->getNumVerts();
	}

	// set up the space for the triangles
	allocateMemory(numVerts, numTriangles);

	// fill up the arrays
	numTriangles = 0;
	numVerts = 0;
	unsigned int d;
	double * vertex;
	double * texture;
	// for each polygon
	for (c=0; c<m_PolygonArray.getNumPolygons(); c++) {
		// fill in the vertices
		for (d=0; d<m_PolygonArray.getPolygon(c)->getNumVerts(); d++) {
			vertex = m_PolygonArray.getPolygon(c)->getVert(d);
			texture = m_PolygonArray.getPolygon(c)->getTexCoord(d);
			m_VertexArray[(numVerts+d)*3+0] = (float)vertex[0];
			m_VertexArray[(numVerts+d)*3+1] = (float)vertex[1];
			m_VertexArray[(numVerts+d)*3+2] = (float)vertex[2];
			m_TextureArray[(numVerts+d)*3+0] = (float)texture[0];
			m_TextureArray[(numVerts+d)*3+1] = (float)texture[1];
			m_TextureArray[(numVerts+d)*3+2] = (float)texture[2];
		}
		// fill in the triangles
		for (d=0; d<m_PolygonArray.getPolygon(c)->getNumTriangles()*3; d++) {
			m_TriangleArray[(numTriangles*3)+d] = numVerts +
				m_PolygonArray.getPolygon(c)->getVertexForTriangles(d);
		}
		numTriangles+=m_PolygonArray.getPolygon(c)->getNumTriangles();
		numVerts+=m_PolygonArray.getPolygon(c)->getNumVerts();
	}
}

// Sets the aspectRatio to a default value.
bool RendererBase::initAspectRatio()
{
	m_RatioX = 1.0;
	m_RatioY = 1.0;
	m_RatioZ = 1.0;
	return true;
}

// Sets all flags to default values.
bool RendererBase::initFlags()
{
	m_BaseInitialized = false;
	return true;
}

// static helper function to concat two matrices
static void concatMatrices(double* result, float* m1, float* m2)
{
    int i;
	
    double mb00, mb01, mb02, mb03,
		
		mb10, mb11, mb12, mb13,
		
		mb20, mb21, mb22, mb23,
		
		mb30, mb31, mb32, mb33;
	
    double mai0, mai1, mai2, mai3;
	
	
	
    mb00 = m2[0];  mb01 = m2[1];
	
    mb02 = m2[2];  mb03 = m2[3];
	
    mb10 = m2[4];  mb11 = m2[5];
	
    mb12 = m2[6];  mb13 = m2[7];
	
    mb20 = m2[8];  mb21 = m2[9];
	
    mb22 = m2[10];  mb23 = m2[11];
	
    mb30 = m2[12];  mb31 = m2[13];
	
    mb32 = m2[14];  mb33 = m2[15];
	
	
	
    for (i = 0; i < 4; i++) {
		
        mai0 = m1[i*4+0];  mai1 = m1[i*4+1];
		
		mai2 = m1[i*4+2];  mai3 = m1[i*4+3];
		
		
		
        result[i*4+0] = (mai0 * mb00 + mai1 * mb10 + mai2 * mb20 + mai3 * mb30);
		
        result[i*4+1] = (mai0 * mb01 + mai1 * mb11 + mai2 * mb21 + mai3 * mb31);
		
        result[i*4+2] = (mai0 * mb02 + mai1 * mb12 + mai2 * mb22 + mai3 * mb32);
		
        result[i*4+3] = (mai0 * mb03 + mai1 * mb13 + mai2 * mb23 + mai3 * mb33);
		
    }

}

// Returns a plane parallel to the view plane.
Plane RendererBase::getViewPlane()
{
	GLfloat modelview[16], projection[16]; 
	double combined[16];

	// inverse transform the plane normal using the opengl matrices
	// the view plane is 0.0, 0.0, -1.0, 0.0 in clip coordinate

	// first, get the modelview and projection matrices
    glGetFloatv (GL_MODELVIEW_MATRIX, modelview);
    glGetFloatv (GL_PROJECTION_MATRIX, projection);

	// then concatinate the projection and modelview matrices
	concatMatrices(combined, modelview, projection);

	// then calculate the plane equation in object space
	// See http://www.opengl.org/developers/faqs/technical/viewcull.c
	// for how this operation was simplified
	Plane plane(
		combined[3]+combined[2], 
		combined[7]+combined[6],
		combined[11]+ combined[10],
		0.0);

	plane.normalizeNormal();
	return plane;
}

// Returns the distance between planes.
double RendererBase::getIntervalWidth() const
{
	double cellWidthX = m_RatioX / m_HintDimX;
	double cellWidthY = m_RatioY / m_HintDimY;
	double cellWidthZ = m_RatioZ / m_HintDimZ;

	// find the minimum cell width
	double minWidth = ( cellWidthX < cellWidthY ? cellWidthX : cellWidthY );
	minWidth = ( minWidth < cellWidthZ ? minWidth : cellWidthZ );

	return minWidth / 2.0 * ((1.0-m_Quality)*(1.0-m_Quality) * 10.0 + 1.0);
}

// Returns a distance that is past the entire volume.
double RendererBase::getFurthestDistance() const
{
	return 0.5 * sqrt( m_RatioX*m_RatioX + m_RatioY*m_RatioY + m_RatioZ*m_RatioZ );
}

// Returns a distance that is before the entire volume.
double RendererBase::getNearestDistance() const
{
	double diagonal = sqrt( m_RatioX*m_RatioX + m_RatioY*m_RatioY + m_RatioZ*m_RatioZ );
	return (-0.5 * diagonal) +
		(m_NearPlane * diagonal);
}

// Computes the polygons that need to be rendered
void RendererBase::computePolygons()
{
	m_PolygonArray.clearPolygons();

	Plane plane = getViewPlane();

	ClipCube cube(m_RatioX, m_RatioY, m_RatioZ,
		m_TextureSubCubeExtent.m_MinX, 
		m_TextureSubCubeExtent.m_MinY, 
		m_TextureSubCubeExtent.m_MinZ, 
		m_TextureSubCubeExtent.m_MaxX, 
		m_TextureSubCubeExtent.m_MaxY, 
		m_TextureSubCubeExtent.m_MaxZ);
	Polygon polygon(0);

	double z;
	for (z = getFurthestDistance(); z>getNearestDistance(); z-=getIntervalWidth()) {
		plane[3] = z;
		if (cube.clipPlane(polygon, plane)) {
			m_PolygonArray.addPolygon(polygon);
		}
	}

	m_NumberOfPlanesRendered = m_PolygonArray.getNumPolygons();
}

