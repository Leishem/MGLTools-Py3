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

// Paletted2DImpl.cpp: implementation of the Paletted2DImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "Paletted2DImpl.h"
#include <math.h>
#include "ClipCube.h"

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Paletted2DImpl::Paletted2DImpl()
{
	m_Initialized = false;
	m_Width = -1;
	m_Height = -1;
	m_Depth = -1;
	m_NumAllocated[0] = 0;
	m_NumAllocated[1] = 0;
	m_NumAllocated[2] = 0;
	m_DataTextureNameX = 0;
	m_DataTextureNameY = 0;
	m_DataTextureNameZ = 0;
	m_ColorMapDirty[0] = true;
	m_ColorMapDirty[1] = true;
	m_ColorMapDirty[2] = true;
	unsigned int c;
	for (c=0; c<256*4; c++) {
		m_ColorMap[c] = 255;
	}

}

Paletted2DImpl::~Paletted2DImpl()
{

}

// Initializes the renderer.  Should be called again if the renderer is
// moved to a different openGL context.  If this returns false, do not try
// to use it to do volumeRendering
bool Paletted2DImpl::initRenderer()
{
	if (!UnshadedBase::initRenderer() || !initExtensions()) {
		m_Initialized = false;
		m_Width = -1;
		m_Height = -1;
		m_Depth = -1;
		return false;
	}
	else {
		m_Initialized = true;
		return true;
	}
}

// Makes the check necessary to determine if this renderer is 
// compatible with the hardware its running on
bool Paletted2DImpl::checkCompatibility() const
{
	MyExtensions tempExtensions;
	if (!tempExtensions.initExtensions("GL_VERSION_1_2 ") && 
		!tempExtensions.initExtensions("GL_SGIS_texture_edge_clamp ")) {
		// if neither is available, we have to bail
		return false;
	}
	return tempExtensions.initExtensions(
										"GL_EXT_paletted_texture "
										);
}

// Uploads colormapped data
bool Paletted2DImpl::uploadColormappedData(const GLubyte* data, int width, int height, int depth)
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// clear previous errors
	GLenum error = glGetError();


	if (width!=m_Width || height!=m_Height || depth!=m_Depth) {
		if (!initTextureNames(width, height, depth)) {
			return false;
		}
		int c;
		for (c=0; c<depth; c++) { // copy the z axis images first
			glBindTexture(GL_TEXTURE_2D, m_DataTextureNameZ[c]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, width, height,
				0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, data+width*height*c);
			setTextureParameters();
		}

		GLubyte* tempYBuffer = new GLubyte[depth*width];
		for (c=0; c<height; c++) { // copy the y axis next
			// first get a slice from the data
			getYSlice(tempYBuffer, data, c, width, height, depth);
			glBindTexture(GL_TEXTURE_2D, m_DataTextureNameY[c]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, depth, width,
				0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, tempYBuffer);
			setTextureParameters();
		}
		delete [] tempYBuffer;


		GLubyte* tempXBuffer = new GLubyte[height*depth];
		for (c=0; c<width; c++) { // copy the x axis next
			// first get a slice from the data
			getXSlice(tempXBuffer, data, c, width, height, depth);
			glBindTexture(GL_TEXTURE_2D, m_DataTextureNameX[c]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, height, depth,
				0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, tempXBuffer);
			setTextureParameters();
		}
		delete [] tempXBuffer;

		// done

	}
	else {

		int c;
		for (c=0; c<depth; c++) { // copy the z axis images first
			glBindTexture(GL_TEXTURE_2D, m_DataTextureNameZ[c]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
				GL_COLOR_INDEX, GL_UNSIGNED_BYTE, data+width*height*c);
			setTextureParameters();
		}

		GLubyte* tempYBuffer = new GLubyte[depth*width];
		for (c=0; c<height; c++) { // copy the y axis next
			// first get a slice from the data
			getYSlice(tempYBuffer, data, c, width, height, depth);
			glBindTexture(GL_TEXTURE_2D, m_DataTextureNameY[c]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depth, width,
				GL_COLOR_INDEX, GL_UNSIGNED_BYTE, tempYBuffer);
			setTextureParameters();
		}
		delete [] tempYBuffer;


		GLubyte* tempXBuffer = new GLubyte[height*depth];
		for (c=0; c<width; c++) { // copy the z axis next
			// first get a slice from the data
			getXSlice(tempXBuffer, data, c, width, height, depth);
			glBindTexture(GL_TEXTURE_2D, m_DataTextureNameX[c]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, height, depth,
				GL_COLOR_INDEX, GL_UNSIGNED_BYTE, tempXBuffer);
			setTextureParameters();
		}
		delete [] tempXBuffer;

		// done

	}

	m_ColorMapDirty[0] = true;
	m_ColorMapDirty[1] = true;
	m_ColorMapDirty[2] = true;

	// save the width height and depth
	m_Width = width;   m_HintDimX = width;
	m_Height = height; m_HintDimY = height;
	m_Depth = depth;   m_HintDimZ = depth;

	// test for error
	error = glGetError();
	if (error == GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}

}


// Tests to see if the given parameters would return an error
bool Paletted2DImpl::testColormappedData(int width, int height, int depth)
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// nothing above 512
	if (width>512 || height>512 || depth>512) {
		return false;
	}

	// clear previous errors
	GLenum error;
	int c =0;
	while (glGetError()!=GL_NO_ERROR && c<10) c++;

	glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, width, height,
		0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, depth, width,
		0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, 0);
	glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, height, depth,
		0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, 0);

	// test for error
	error = glGetError();
	if (error == GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}
}

// Uploads the transfer function for the colormapped data
bool Paletted2DImpl::uploadColorMap(const GLubyte* colorMap)
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// clear previous errors
	GLenum error = glGetError();
/*
	int c;
	for (c=0; c<m_Depth; c++) { // copy the z axis images first
		glBindTexture(GL_TEXTURE_2D, m_DataTextureNameZ[c]);
		m_Extensions.glColorTableEXT(GL_TEXTURE_2D, GL_RGBA8, 256, GL_RGBA, 
			GL_UNSIGNED_BYTE, colorMap);
	}

	for (c=0; c<m_Height; c++) { // copy the y axis next
		glBindTexture(GL_TEXTURE_2D, m_DataTextureNameY[c]);
		m_Extensions.glColorTableEXT(GL_TEXTURE_2D, GL_RGBA8, 256, GL_RGBA, 
			GL_UNSIGNED_BYTE, colorMap);
	}

	for (c=0; c<m_Width; c++) { // copy the z axis next
		glBindTexture(GL_TEXTURE_2D, m_DataTextureNameX[c]);
		m_Extensions.glColorTableEXT(GL_TEXTURE_2D, GL_RGBA8, 256, GL_RGBA, 
			GL_UNSIGNED_BYTE, colorMap);
	}
*/
	unsigned int c;
	for (c=0; c<256*4; c++) {
		m_ColorMap[c] = colorMap[c];
	}
	m_ColorMapDirty[0] = true;
	m_ColorMapDirty[1] = true;
	m_ColorMapDirty[2] = true;

	// test for error
	error = glGetError();
	if (error == GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}
}

// Performs the actual rendering.
bool Paletted2DImpl::renderVolume()
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// set up the state
	glPushAttrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_TABLE); //  This is for the paletted_texture color table
	//glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDepthMask( GL_FALSE );

	computePolygons();

	renderPolygons();

	// restore the state
	glPopAttrib();

	return true;

}

// Computes the polygons that need to be rendered
void Paletted2DImpl::computePolygons()
{
	m_PolygonArray.clearPolygons();

	// chose a plane with normal parallel to the primary viewing direction
	Plane plane = getViewPlane();
	if (fabs(plane.a())>fabs(plane.b()) && fabs(plane.a())>fabs(plane.c())) {
		plane.b()=0.0;
		plane.c()=0.0;
		plane.d()=0.0;
		plane.normalizeNormal();
		m_RenderDirection = 0;
	}
	else if (fabs(plane.b())>fabs(plane.c())) {
		plane.a()=0.0;
		plane.c()=0.0;
		plane.d()=0.0;
		plane.normalizeNormal();
		m_RenderDirection = 1;
	}
	else {
		plane.a()=0.0;
		plane.b()=0.0;
		plane.d()=0.0;
		plane.normalizeNormal();
		m_RenderDirection = 2;
	}

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

// Renders the computed polygons
void Paletted2DImpl::renderPolygons()
{
	GLuint* textureNamesArray[] = {m_DataTextureNameX, m_DataTextureNameY, m_DataTextureNameZ};
	GLuint* textureNames = textureNamesArray[m_RenderDirection];
	unsigned int numTexturesArray[] = {m_Width, m_Height, m_Depth};
	unsigned int numTextures = numTexturesArray[m_RenderDirection];
	unsigned int coordSwitchArray[3][3] = { {1, 2}, {2, 0}, {0, 1} };
	unsigned int* coordSwitch = coordSwitchArray[m_RenderDirection];

	glEnable(GL_TEXTURE_2D);
	unsigned int c;
	for (c=0; c<m_PolygonArray.getNumPolygons(); c++) {
		// determine the z coord
		double z = m_PolygonArray.getPolygon(c)->getTexCoord(0)[m_RenderDirection];
		z = z*numTextures;
		int texnum = (int)z;
		texnum = (texnum<0?0:texnum);
		texnum = ((unsigned int)texnum>numTextures-1?numTextures-1:texnum);
		glBindTexture(GL_TEXTURE_2D, textureNames[texnum]);
		if (m_ColorMapDirty[m_RenderDirection]) {
			m_Extensions.glColorTableEXT(GL_TEXTURE_2D, GL_RGBA8, 256, GL_RGBA, 
				GL_UNSIGNED_BYTE, m_ColorMap);
		}
		glBegin(GL_POLYGON);
		unsigned int d;
		for (d=0; d<m_PolygonArray.getPolygon(c)->getNumVerts(); d++) {
			double* texCoord = m_PolygonArray.getPolygon(c)->getTexCoord(d);
			double xCoord = texCoord[coordSwitch[0]];
			double yCoord = texCoord[coordSwitch[1]];

			glTexCoord2d(xCoord, yCoord);
			glVertex3dv(m_PolygonArray.getPolygon(c)->getVert(d));
		}

		glEnd();
	}
	m_ColorMapDirty[m_RenderDirection] = false;
}

// Initializes the necessary extensions.
bool Paletted2DImpl::initExtensions()
{
	if (!m_Extensions.initExtensions("GL_VERSION_1_2 ") && 
		!m_Extensions.initExtensions("GL_SGIS_texture_edge_clamp ")) {
		// if neither is available, we have to bail
		return false;
	}
	return m_Extensions.initExtensions(
										"GL_EXT_paletted_texture "
										);
}

// Gets the opengl texture IDs
bool Paletted2DImpl::initTextureNames(unsigned int x, unsigned int y, unsigned int z)
{
	// clear previous errors
	GLenum error = glGetError();

	// x dimension
	if (x>m_NumAllocated[0]) {
		GLuint* temp = new GLuint[x];
		// delete old
		if (m_NumAllocated[0]>0) glDeleteTextures(m_NumAllocated[0], m_DataTextureNameX);
		//unsigned int c;
		//for (c=0; c<m_NumAllocated[0]; c++) {
		//	temp[c] = m_DataTextureNameX[c];
		//}
		// delete old
		delete [] m_DataTextureNameX;
		m_DataTextureNameX = temp;
		// get the new names
		glGenTextures(x, m_DataTextureNameX);
		m_NumAllocated[0] = x;
	}

	// y dimension
	if (y>m_NumAllocated[1]) {
		GLuint* temp = new GLuint[y];
		// delete old
		if (m_NumAllocated[1]>0) glDeleteTextures(m_NumAllocated[1], m_DataTextureNameY);
		//unsigned int c;
		//for (c=0; c<m_NumAllocated[1]; c++) {
		//	temp[c] = m_DataTextureNameY[c];
		//}
		// delete old
		delete [] m_DataTextureNameY;
		m_DataTextureNameY = temp;
		// get the new names
		glGenTextures(y, m_DataTextureNameY);
		m_NumAllocated[1] = y;
	}

	// z dimension
	if (z>m_NumAllocated[2]) {
		GLuint* temp = new GLuint[z];
		// delete old
		if (m_NumAllocated[2]>0) glDeleteTextures(m_NumAllocated[2], m_DataTextureNameZ);
		//unsigned int c;
		//for (c=0; c<m_NumAllocated[2]; c++) {
		//	temp[c] = m_DataTextureNameZ[c];
		//}
		// delete old
		delete [] m_DataTextureNameZ;
		m_DataTextureNameZ = temp;
		// get the new names
		glGenTextures(z, m_DataTextureNameZ);
		m_NumAllocated[2] = z;
	}

	// test for error
	error = glGetError();
	if (error==GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}
}

// Sets the currently bound texture parameters
void Paletted2DImpl::setTextureParameters()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

// Gets a single slice along the y-axis
void Paletted2DImpl::getYSlice(GLubyte* dest, const GLubyte* source, unsigned int sliceNum, unsigned int width, unsigned int height, unsigned int depth)
{
	unsigned int sourceZ, sourceX, sourceY=sliceNum;
	unsigned int destX, destY;

	for (sourceZ=0; sourceZ<depth; sourceZ++) {
		for (sourceX=0; sourceX<width; sourceX++) {
			destX = sourceZ;
			destY = sourceX;
			dest[destY*depth + destX] =
				source[sourceZ*width*height + sourceY*width + sourceX];
		}
	}
}

		
// Gets a single slice along the x-axis
void Paletted2DImpl::getXSlice(GLubyte* dest, const GLubyte* source, unsigned int sliceNum, unsigned int width, unsigned int height, unsigned int depth)
{
	unsigned int sourceY, sourceZ, sourceX=sliceNum;
	unsigned int destX, destY;

	for (sourceZ=0; sourceZ<depth; sourceZ++) {
		for (sourceY=0; sourceY<height; sourceY++) {
			destX = sourceY;
			destY = sourceZ;
			dest[destY*height + destX] =
				source[sourceZ*width*height + sourceY*width + sourceX];
		}
	}
}


