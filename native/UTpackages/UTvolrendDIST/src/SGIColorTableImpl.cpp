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

// SGIColorTableImpl.cpp: implementation of the SGIColorTableImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "SGIColorTableImpl.h"

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SGIColorTableImpl::SGIColorTableImpl()
{
	m_Initialized = false;
	m_Width = -1;
	m_Height = -1;
	m_Depth = -1;
}

SGIColorTableImpl::~SGIColorTableImpl()
{

}

// Initializes the renderer.  Should be called again if the renderer is
// moved to a different openGL context.  If this returns false, do not try
// to use it to do volumeRendering
bool SGIColorTableImpl::initRenderer()
{
	if (!UnshadedBase::initRenderer() || !initExtensions() || !initTextureNames()) {
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
bool SGIColorTableImpl::checkCompatibility() const
{
	MyExtensions tempExtensions;
	if (!tempExtensions.initExtensions("GL_VERSION_1_2 ") && 
		!tempExtensions.initExtensions("GL_SGIS_texture_edge_clamp ")) {
		// if neither is available, we have to bail
		return false;
	}
	return tempExtensions.initExtensions(
										"GL_EXT_texture3D "
										"GL_SGI_texture_color_table "
										"GL_SGI_color_table "
										);
}

// Uploads colormapped data
bool SGIColorTableImpl::uploadColormappedData(const GLubyte* data, int width, int height, int depth)
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// clear previous errors
	GLenum error = glGetError();


	glBindTexture(GL_TEXTURE_3D_EXT, m_DataTextureName);

	if (width!=m_Width || height!=m_Height || depth!=m_Depth) {
		m_Extensions.glTexImage3DEXT(GL_TEXTURE_3D_EXT, 0, GL_INTENSITY8, width, height,
			depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	}
	else {
		m_Extensions.glTexSubImage3DEXT(GL_TEXTURE_3D_EXT, 0, 0,0,0, width, height,
			depth, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		
	}

	glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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
bool SGIColorTableImpl::testColormappedData(int width, int height, int depth)
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

	m_Extensions.glTexImage3DEXT(GL_PROXY_TEXTURE_3D_EXT, 0, GL_INTENSITY, width, height,
		depth, 0, GL_INTENSITY, GL_UNSIGNED_BYTE, 0);

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
bool SGIColorTableImpl::uploadColorMap(const GLubyte* colorMap)
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// clear previous errors
	GLenum error = glGetError();

	glBindTexture(GL_TEXTURE_3D_EXT, m_DataTextureName);
	m_Extensions.glColorTableSGI(GL_TEXTURE_COLOR_TABLE_SGI, GL_RGBA8, 256, 
		GL_RGBA, GL_UNSIGNED_BYTE, colorMap);

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
bool SGIColorTableImpl::renderVolume()
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// set up the state
	glPushAttrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
	//glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDepthMask( GL_FALSE );

	glEnable(GL_TEXTURE_3D_EXT);
	glBindTexture(GL_TEXTURE_3D_EXT, m_DataTextureName);

	computePolygons();

	convertToTriangles();

	renderTriangles();

	// restore the state
	glPopAttrib();

	return true;

}

// Initializes the necessary extensions.
bool SGIColorTableImpl::initExtensions()
{
	if (!m_Extensions.initExtensions("GL_VERSION_1_2 ") && 
		!m_Extensions.initExtensions("GL_SGIS_texture_edge_clamp ")) {
		// if neither is available, we have to bail
		return false;
	}
	return m_Extensions.initExtensions(
										"GL_EXT_texture3D "
										"GL_SGI_texture_color_table "
										"GL_SGI_color_table "
										);
}

// Gets the opengl texture IDs
bool SGIColorTableImpl::initTextureNames()
{
	// clear previous errors
	GLenum error = glGetError();

	// get the names
	glGenTextures(1, &m_DataTextureName);

	// test for error
	error = glGetError();
	if (error==GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}
}

// Render the actual triangles
void SGIColorTableImpl::renderTriangles()
{
	// set up the client render state
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	glTexCoordPointer(3, GL_FLOAT, 0, m_TextureArray);
	glVertexPointer(3, GL_FLOAT, 0, m_VertexArray);
	//glColorPointer(3, GL_FLOAT, 0, m_TextureArray);

	// render the triangles
	glDrawElements(GL_TRIANGLES, m_NumTriangles*3, GL_UNSIGNED_INT, m_TriangleArray);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
}


