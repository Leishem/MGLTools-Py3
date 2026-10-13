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

// Renderer.cpp: implementation of the Renderer class.
//
//////////////////////////////////////////////////////////////////////

#include "Renderer.h"

#include <math.h>

#include "Polygon.h"
#include "ClipCube.h"

#include "PalettedImpl.h"
#include "FragmentProgramImpl.h"
#include "FragmentProgramARBImpl.h"
#include "SGIColorTableImpl.h"
#include "SimpleRGBAImpl.h"
#include "SimpleRGBA2DImpl.h"
#include "Paletted2DImpl.h"

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Renderer::Renderer()
{
	m_RGBA = 0;
	m_Colormapped = 0;
	initFlags();
}

Renderer::~Renderer()
{
	delete m_Colormapped;
	delete m_RGBA;
}

bool Renderer::initFlags()
{
	m_UseColormapped = false;
	m_DataLoaded = false;
	return true;
}

bool Renderer::initRenderer()
{
	bool colormapped = initColormappedRenderer();;
	bool rgba = initRGBARenderer();
	if (colormapped &&
		rgba) {
		m_DataLoaded = false;
		return true;
	}
	else {
		return false;
	}
}

bool Renderer::setTextureSubCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
{
	if (m_Colormapped) m_Colormapped->setTextureSubCube(minX, minY, minZ, maxX, maxY, maxZ);
	if (m_RGBA) m_RGBA->setTextureSubCube(minX, minY, minZ, maxX, maxY, maxZ);
	return true;
}

bool Renderer::setQuality(double quality)
{
	if (m_Colormapped) m_Colormapped->setQuality(quality);
	if (m_RGBA) m_RGBA->setQuality(quality);
	return true;
}

double Renderer::getQuality() const
{
	if (m_UseColormapped && m_Colormapped)
		return m_Colormapped->getQuality();
	else if (m_RGBA)
		return m_RGBA->getQuality();
	else 
		return 0;
}

bool Renderer::setNearPlane(double nearPlane)
{
	if (m_Colormapped) m_Colormapped->setNearPlane(nearPlane);
	if (m_RGBA) m_RGBA->setNearPlane(nearPlane);
	return true;
}

double Renderer::getNearPlane()
{
	if (m_Colormapped) return m_Colormapped->getNearPlane();
	else if (m_RGBA) return m_RGBA->getNearPlane();
	else return 1.0;
}

bool Renderer::setAspectRatio(double ratioX, double ratioY, double ratioZ)
{
	if (m_Colormapped) m_Colormapped->setAspectRatio(ratioX, ratioY, ratioZ);
	if (m_RGBA) m_RGBA->setAspectRatio(ratioX, ratioY, ratioZ);
	return true;
}


bool Renderer::isShadedRenderingAvailable() const
{
	return false;
}

bool Renderer::enableShadedRendering()
{
	return false;
}

bool Renderer::disableShadedRendering()
{
	return true;
}


bool Renderer::uploadColorMappedData(const GLubyte* data, int width, int height, int depth)
{
	if (m_Colormapped && m_Colormapped->uploadColormappedData(data, width, height, depth)) {
		m_UseColormapped = true;
		m_DataLoaded = true;
		return true;
	}
	else {
		return false;
	}
}

bool Renderer::uploadColorMappedDataWithBorder(const GLubyte* data, int width, int height, int depth)
{
	return false;
	/*
#ifdef GL_EXT_paletted_texture

	// clear previous errors
	GLenum error = glGetError();

	// save the width height and depth
	m_Width = width;
	m_Height = height;
	m_Depth = depth;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_3D, m_DataTextureName);
	m_Extensions.glTexImage3D(GL_TEXTURE_3D, 0, GL_COLOR_INDEX8_EXT, width, height,
		depth, 1, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// test for error
	error = glGetError();
	if (error == GL_NO_ERROR) {
		m_UsingColorMap = true;
		m_DataLoaded = true;
		return true;
	}
	else {
		m_DataLoaded = false;
		return false;
	}
#elif GL_SGI_texture_color_table
	// use SGI's color table system
	// clear previous errors
	GLenum error = glGetError();

	// save the width height and depth
	m_Width = width;
	m_Height = height;
	m_Depth = depth;

	glBindTexture(GL_TEXTURE_3D, m_DataTextureName);
	glTexImage3D(GL_TEXTURE_3D, 1, GL_INTENSITY, width, height,
		depth, 0, GL_INTENSITY, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// test for error
	error = glGetError();
	if (error == GL_NO_ERROR) {
		glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
		m_UsingColorMap = true;
		m_DataLoaded = true;
		return true;
	}
	else {
		m_DataLoaded = false;
		return false;
	}

#else
	return false;
#endif // GL_EXT_paletted_texture
	*/
}

bool Renderer::testColorMappedData(int width, int height, int depth)
{
	return m_Colormapped->testColormappedData(width, height, depth);
}

bool Renderer::testColorMappedDataWithBorder(int width, int height, int depth)
{
	return false;
	/*
#ifdef GL_EXT_paletted_texture
	// nothing above 512
	if (width>514 || height>514 || depth>514) {
		return false;
	}

	// clear previous errors
	GLenum error;
	int c =0;
	while (glGetError()!=GL_NO_ERROR && c<10) c++;

	m_Extensions.glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_COLOR_INDEX8_EXT, width, height,
		depth, 1, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, 0);

	// test for error
	error = glGetError();
	if (error == GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}
#elif GL_SGI_texture_color_table
	// use SGI's color table system
	// clear previous errors
	GLenum error;
	int c =0;
	while (glGetError()!=GL_NO_ERROR && c<10) c++;

	glTexImage3D(GL_PROXY_TEXTURE_3D_EXT, 1, GL_INTENSITY, width, height,
		depth, 0, GL_INTENSITY, GL_UNSIGNED_BYTE, 0);

	// test for error
	error = glGetError();
	if (error == GL_NO_ERROR) {
		return true;
	}
	else {
		return false;
	}

#else
	return false;
#endif // GL_EXT_paletted_texture
	*/
}

bool Renderer::uploadRGBAData(const GLubyte* data, int width, int height, int depth)
{
	if (m_RGBA && m_RGBA->uploadRGBAData(data, width, height, depth)) {
		m_UseColormapped = false;
		m_DataLoaded = true;
		return true;
	}
	else {
		return false;
	}
}

bool Renderer::uploadGradients(const GLubyte* data, int width, int height, int depth)
{
	return false;
}

bool Renderer::calculateGradientsFromDensities(const GLubyte* data, int width, int height, int depth)
{
	return false;
	/*
	GLubyte* grads = new GLubyte[width*height*depth*4];

	double dx,dy,dz,length;

	int k, j, i, inputindex, outputindex;

	int negXOffset, posXOffset;
	int negYOffset, posYOffset;
	int negZOffset, posZOffset;

	for (k=0; k<depth; k++) {
		if (k==0) { // border offset
			negZOffset = 0; posZOffset =  width*height;
		}
		else if (k==depth-1) { // border offset
			negZOffset = -width*height; posZOffset = 0;
		}
		else { // normal offset
			negZOffset = -width*height; posZOffset =  width*height;
		}
		for (j=0; j<height; j++) {
			if (j==0) { // border offset
				negYOffset = 0; posYOffset =  width;
			}
			else if (j==height-1) { // border offset
				negYOffset = -width; posYOffset = 0;
			}				
			else { // normal offset
				negYOffset = -width; posYOffset =  width;
			}


			// do border case
			negXOffset = 0; posXOffset =  1;
			inputindex = k*width*height+j*width;
			outputindex=k*width*height*4+j*width*4;
			dx = data[inputindex+negXOffset] - data[inputindex+posXOffset];
			dy = data[inputindex+negYOffset] - data[inputindex+posYOffset];
			dz = data[inputindex+negZOffset] - data[inputindex+posZOffset];
			length = sqrt(dx*dx+dy*dy+dz*dz);


			grads[outputindex + 0] = (unsigned char)(dx/length * 127.0)+127;
			grads[outputindex + 1] = (unsigned char)(dy/length * 127.0)+127;
			grads[outputindex + 2] = (unsigned char)(dz/length * 127.0)+127;
			grads[outputindex + 3] = 255;
			
			// do normal case
			negXOffset = -1; posXOffset =  1;
			for (i=1, inputindex=k*width*height+j*width+1, outputindex=k*width*height*4+j*width*4+4; 
				i<width-2; 
				i++, inputindex++, outputindex+=4) {
				dx = data[inputindex+negXOffset] - data[inputindex+posXOffset];
				dy = data[inputindex+negYOffset] - data[inputindex+posYOffset];
				dz = data[inputindex+negZOffset] - data[inputindex+posZOffset];
				length = sqrt(dx*dx+dy*dy+dz*dz);


				grads[outputindex + 0] = (unsigned char)(dx/length * 127.0)+127;
				grads[outputindex + 1] = (unsigned char)(dy/length * 127.0)+127;
				grads[outputindex + 2] = (unsigned char)(dz/length * 127.0)+127;
				grads[outputindex + 3] = 255;
			}
			
			// do border case
			negXOffset = -1; posXOffset = 0;
			// use the inputindex and outputindex coming off the for loop
			dx = data[inputindex+negXOffset] - data[inputindex+posXOffset];
			dy = data[inputindex+negYOffset] - data[inputindex+posYOffset];
			dz = data[inputindex+negZOffset] - data[inputindex+posZOffset];
			length = sqrt(dx*dx+dy*dy+dz*dz);


			grads[outputindex + 0] = (unsigned char)(dx/length * 127.0)+127;
			grads[outputindex + 1] = (unsigned char)(dy/length * 127.0)+127;
			grads[outputindex + 2] = (unsigned char)(dz/length * 127.0)+127;
			grads[outputindex + 3] = 255;

			
		}
		
	}
				
	bool retval = uploadGradients(grads, width, height, depth);
	delete [] grads;
	return retval;
	*/
}

bool Renderer::uploadColorMap(const GLubyte* colorMap)
{
	return (m_Colormapped && m_Colormapped->uploadColorMap(colorMap));
}

int Renderer::getNumberOfPlanesRendered() const
{
	if (m_UseColormapped && m_Colormapped)
		return m_Colormapped->getNumberOfPlanesRendered();
	else if (m_RGBA)
		return m_RGBA->getNumberOfPlanesRendered();
	else 
		return 0;
}

bool Renderer::renderVolume()
{
	if (m_UseColormapped)
		return (m_Colormapped?m_Colormapped->renderVolume():false);
	else
		return (m_RGBA?m_RGBA->renderVolume():false);
}

bool Renderer::initColormappedRenderer()
{
	
	// first we try the paletted version which we know works on 
	// Nvidia
	m_Colormapped = new PalettedImpl;
	if (m_Colormapped->initRenderer()) {
		return true;
	}
	// failed
	delete m_Colormapped;
	m_Colormapped = 0;

	// looks like NVIDIA might not support the paletted texture
	// extension any more, this is the alternative
	m_Colormapped = new FragmentProgramImpl;
	if (m_Colormapped->initRenderer()) {
		return true;
	}
	// failed
	delete m_Colormapped;
	m_Colormapped = 0;

	// This should work on ATI's and NVidia cards
	m_Colormapped = new FragmentProgramARBImpl;
	if (m_Colormapped->initRenderer()) {
		return true;
	}
	// failed
	delete m_Colormapped;
	m_Colormapped = 0;

	// try a 2d paletted version
	m_Colormapped = new Paletted2DImpl;
	if (m_Colormapped->initRenderer()) {
		return true;
	}
	// failed
	delete m_Colormapped;
	m_Colormapped = 0;

	// next we try the sgi version
	m_Colormapped = new SGIColorTableImpl;
	if (m_Colormapped->initRenderer()) {
		return true;
	}

	// failed, out of options
	delete m_Colormapped;
	m_Colormapped = 0;
	return false;
}

bool Renderer::initRGBARenderer()
{

	// this should work on most platforms
	m_RGBA = new SimpleRGBAImpl;
	if (m_RGBA->initRenderer()) {
		return true;
	}

	// failed
	delete m_RGBA;
	m_RGBA = 0;

	m_RGBA = new SimpleRGBA2DImpl;
	if (m_RGBA->initRenderer()) {
		return true;
	}
	// failed
	delete m_RGBA;
	m_RGBA = 0;
	return false;
}

