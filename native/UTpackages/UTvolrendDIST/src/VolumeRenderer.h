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
  along with Volume Rover; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// VolumeRenderer.h: interface for the VolumeRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUMERENDERER_H__ADD2CDB1_7261_4BA2_8E20_865DD924B4D0__INCLUDED_)
#define AFX_VOLUMERENDERER_H__ADD2CDB1_7261_4BA2_8E20_865DD924B4D0__INCLUDED_

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#if defined(__APPLE__)
# include <gl.h>
#else
# include <GL/gl.h>
#endif

namespace OpenGLVolumeRendering {
	class Renderer;
}

/** Users of the volume library should use this class.  It uses Renderer to create 
 *  colormapped and non-colormapped renderers.
 */
class VolumeRenderer  
{
public:
	VolumeRenderer();
	VolumeRenderer(const VolumeRenderer& copy);
	VolumeRenderer& operator=(const VolumeRenderer& copy);

	virtual ~VolumeRenderer();

	bool initRenderer();
	
	bool setAspectRatio(double ratioX, double ratioY, double ratioZ);
	bool setTextureSubCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);

	bool setQuality(double quality);
	double getQuality() const;

	bool setNearPlane(double nearPlane);
	double getNearPlane();

	bool isShadedRenderingAvailable() const;
	bool enableShadedRendering();
	bool disableShadedRendering();

	bool uploadColorMappedData(const GLubyte* data, int width, int height, int depth);
	bool uploadColorMappedDataWithBorder(const GLubyte* data, int width, int height, int depth);
	bool testColorMappedData(int width, int height, int depth);
	bool testColorMappedDataWithBorder(int width, int height, int depth);
	bool uploadRGBAData(const GLubyte* data, int width, int height, int depth);
	bool uploadGradients(const GLubyte* data, int width, int height, int depth);
	bool calculateGradientsFromDensities(const GLubyte* data, int width, int height, int depth);

	bool uploadColorMap(const GLubyte* colorMap);

	int getNumberOfPlanesRendered() const;

	bool renderVolume();

private:
	OpenGLVolumeRendering::Renderer* m_PrivateRenderer;
};

#endif // !defined(AFX_VOLUMERENDERER_H__ADD2CDB1_7261_4BA2_8E20_865DD924B4D0__INCLUDED_)
