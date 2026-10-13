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

// OpenGLVolumeUnshadedBase.h: interface for the OpenGLVolumeUnshadedBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLVOLUMEUNSHADEDBASE_H__46F9B18E_A365_4CE9_9596_12017FC75BA2__INCLUDED_)
#define AFX_OPENGLVOLUMEUNSHADEDBASE_H__46F9B18E_A365_4CE9_9596_12017FC75BA2__INCLUDED_

#include "RendererBase.h"

namespace OpenGLVolumeRendering {

	/// The base class for colormapped volume renders
	class UnshadedBase : public RendererBase  
	{
	public:
		UnshadedBase();
		virtual ~UnshadedBase();

		/// Initializes the renderer.  Should be called again if the renderer is
		/// moved to a different openGL context.  If this returns false, do not try
		/// to use it to do volumeRendering
		virtual bool initRenderer();

		/// Uploads colormapped data
		virtual bool uploadColormappedData(const GLubyte* data, int width, int height, int depth) = 0;

		/// Tests to see if the given parameters would return an error
		virtual bool testColormappedData(int width, int height, int depth) = 0;

		/// Uploads the transfer function for the colormapped data
		virtual bool uploadColorMap(const GLubyte* colorMap) = 0;
	protected:	
	};

};

#endif // !defined(AFX_OPENGLVOLUMEUNSHADEDBASE_H__46F9B18E_A365_4CE9_9596_12017FC75BA2__INCLUDED_)
