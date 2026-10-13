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

// FragmentProgramImpl.cpp: implementation of the FragmentProgramImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "FragmentProgramImpl.h"
#include <string.h>
using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FragmentProgramImpl::FragmentProgramImpl()
{
	m_Initialized = false;
	m_Width = -1;
	m_Height = -1;
	m_Depth = -1;
}

FragmentProgramImpl::~FragmentProgramImpl()
{

}

// Initializes the renderer.  Should be called again if the renderer is
// moved to a different openGL context.  If this returns false, do not try
// to use it to do volumeRendering
bool FragmentProgramImpl::initRenderer()
{
	if (!UnshadedBase::initRenderer() || !initExtensions() || !initTextureNames() || !initFragmentProgram()) {
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
bool FragmentProgramImpl::checkCompatibility() const
{
	return MyExtensions::checkExtensions(
		"GL_VERSION_1_2 "
		"GL_NV_vertex_program "
		"GL_NV_fragment_program "
		"GL_ARB_multitexture "
		);
}

// Uploads colormapped data
bool FragmentProgramImpl::uploadColormappedData(const GLubyte* data, int width, int height, int depth)
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// clear previous errors
	GLenum error = glGetError();


	glBindTexture(GL_TEXTURE_3D, m_DataTextureName);

	if (width!=m_Width || height!=m_Height || depth!=m_Depth) {
		m_Extensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, width, height,
			depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	}
	else {
		m_Extensions.glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, width, height,
			depth, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		
	}

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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
bool FragmentProgramImpl::testColormappedData(int width, int height, int depth)
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

	m_Extensions.glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_LUMINANCE, width, height,
		depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

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
bool FragmentProgramImpl::uploadColorMap(const GLubyte* colorMap)
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// clear previous errors
	GLenum error = glGetError();

	glBindTexture(GL_TEXTURE_1D, m_TransferTextureName);

	glTexImage1D(GL_TEXTURE_1D, 0, 4, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorMap);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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
bool FragmentProgramImpl::renderVolume()
{
	// bail if we haven't been initialized properly
	if (!m_Initialized) return false;

	// set up the state
	glPushAttrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	// get the fragment program ready
	glEnable(GL_FRAGMENT_PROGRAM_NV);
	m_Extensions.glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, m_FragmentProgramName);

	//glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDepthMask( GL_FALSE );

	// bind the transfer function
	m_Extensions.glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D, m_TransferTextureName);

	// bind the data texture
	m_Extensions.glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, m_DataTextureName);

	computePolygons();

	convertToTriangles();

	renderTriangles();

	// unbind the fragment program
	m_Extensions.glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, 0);

	// restore the state
	glPopAttrib();

	return true;

}

// Initializes the necessary extensions.
bool FragmentProgramImpl::initExtensions()
{
	return m_Extensions.initExtensions(
										"GL_VERSION_1_2 "
										"GL_NV_vertex_program "
										"GL_NV_fragment_program "
										"GL_ARB_multitexture "
										);
}

// Gets the opengl texture IDs
bool FragmentProgramImpl::initTextureNames()
{
	// clear previous errors
	GLenum error = glGetError();

	// get the names
	glGenTextures(1, &m_DataTextureName);
	glGenTextures(1, &m_TransferTextureName);

	// test for error
	error = glGetError();
	if (error==GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}
}

// Gets the fragment program ready
bool FragmentProgramImpl::initFragmentProgram()
{
	// clear previous errors
	GLenum error = glGetError();

	m_Extensions.glGenProgramsNV(1, &m_FragmentProgramName);

	const GLubyte program[] =	"!!FP1.0\n"
								"TEX  R0.x, f[TEX0].xyzx, TEX0, 3D;\n"
								"TEX  o[COLR], R0.x, TEX1, 1D;\n"
								"END\n";
	int len = strlen((const char*)program);
	m_Extensions.glLoadProgramNV(GL_FRAGMENT_PROGRAM_NV, m_FragmentProgramName, len, program);

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
void FragmentProgramImpl::renderTriangles()
{
	// set up the client render state
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glTexCoordPointer(3, GL_FLOAT, 0, m_TextureArray);
	glVertexPointer(3, GL_FLOAT, 0, m_VertexArray);

	// render the triangles
	glDrawElements(GL_TRIANGLES, m_NumTriangles*3, GL_UNSIGNED_INT, m_TriangleArray);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
