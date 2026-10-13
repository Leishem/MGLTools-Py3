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

// Paletted2DImpl.h: interface for the Paletted2DImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PALETTED2DIMPL_H__92BAADF9_254C_4539_9BDA_4D1D50EBBFBC__INCLUDED_)
#define AFX_PALETTED2DIMPL_H__92BAADF9_254C_4539_9BDA_4D1D50EBBFBC__INCLUDED_

#include "UnshadedBase.h"

#include "MyExtensions.h"

namespace OpenGLVolumeRendering {

	/// A volume renderer which uses the paletted textures extension to do color
	/// mapping. Only requires support for 2D textures
	class Paletted2DImpl : public UnshadedBase  
	{
	public:
		Paletted2DImpl();
		virtual ~Paletted2DImpl();

		// Initializes the renderer.  Should be called again if the renderer is
		// moved to a different openGL context.  If this returns false, do not try
		// to use it to do volumeRendering
		virtual bool initRenderer();

		// Makes the check necessary to determine if this renderer is 
		// compatible with the hardware its running on
		virtual bool checkCompatibility() const;

		// Uploads colormapped data
		virtual bool uploadColormappedData(const GLubyte* data, int width, int height, int depth);

		// Tests to see if the given parameters would return an error
		virtual bool testColormappedData(int width, int height, int depth);

		// Uploads the transfer function for the colormapped data
		virtual bool uploadColorMap(const GLubyte* colorMap);

		// Performs the actual rendering.
		virtual bool renderVolume();
	protected:
		// Remembers the uploaded width height and depth
		int m_Width, m_Height, m_Depth;

		// The opengl texture IDs
		unsigned int m_NumAllocated[3];
		bool m_ColorMapDirty[3];
		GLubyte m_ColorMap[256*4];
		GLuint* m_DataTextureNameX;
		GLuint* m_DataTextureNameY;
		GLuint* m_DataTextureNameZ;
		unsigned int m_RenderDirection;

		// Holds the pointers to the extension functions
		MyExtensions m_Extensions;

		// Flag indicating if we were successfully initialized
		bool m_Initialized;

		// Computes the polygons that need to be rendered
		virtual void computePolygons();

		// Renders the computed polygons
		virtual void renderPolygons();

		// Initializes the necessary extensions.
		virtual bool initExtensions();

		// Gets the opengl texture IDs
		bool initTextureNames(unsigned int x, unsigned int y, unsigned int z);

		// Sets the currently bound texture parameters
		void setTextureParameters();
		
		// Gets a single slice along the y-axis
		void getYSlice(GLubyte* dest, const GLubyte* source, unsigned int sliceNum, unsigned int width, unsigned int height, unsigned int depth);
		
		// Gets a single slice along the x-axis
		void getXSlice(GLubyte* dest, const GLubyte* source, unsigned int sliceNum, unsigned int width, unsigned int height, unsigned int depth);
	};

};

#endif // !defined(AFX_PALETTED2DIMPL_H__92BAADF9_254C_4539_9BDA_4D1D50EBBFBC__INCLUDED_)
