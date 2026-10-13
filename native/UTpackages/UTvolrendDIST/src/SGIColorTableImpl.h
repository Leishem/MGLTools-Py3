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

// OpenGLVolumeSGIColorTableImpl.h: interface for the OpenGLVolumeSGIColorTableImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLVOLUMESGICOLORTABLEIMPL_H__4EBB934A_B53C_4057_92BA_83BC07C67CDD__INCLUDED_)
#define AFX_OPENGLVOLUMESGICOLORTABLEIMPL_H__4EBB934A_B53C_4057_92BA_83BC07C67CDD__INCLUDED_

#include "UnshadedBase.h"

namespace OpenGLVolumeRendering {

	/** A volume renderer which uses SGI's color table extension to do color mapping */
	class SGIColorTableImpl : public UnshadedBase  
	{
	public:
		SGIColorTableImpl();
		virtual ~SGIColorTableImpl();

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

		// The opengl texture ID
		GLuint m_DataTextureName;

		// Holds the pointers to the extension functions
		MyExtensions m_Extensions;

		// Flag indicating if we were successfully initialized
		bool m_Initialized;

		// Initializes the necessary extensions.
		virtual bool initExtensions();

		// Gets the opengl texture IDs
		bool initTextureNames();

		// Render the actual triangles
		void renderTriangles();
	};

};

#endif // !defined(AFX_OPENGLVOLUMESGICOLORTABLEIMPL_H__4EBB934A_B53C_4057_92BA_83BC07C67CDD__INCLUDED_)
