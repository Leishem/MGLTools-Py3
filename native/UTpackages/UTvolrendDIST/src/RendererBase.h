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

// OpenGLVolumeRendererBase.h: interface for the OpenGLVolumeRendererBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLVOLUMERENDERERBASE_H__71ABAABC_1CAE_4AE1_BC1E_7AFA7F1A65F5__INCLUDED_)
#define AFX_OPENGLVOLUMERENDERERBASE_H__71ABAABC_1CAE_4AE1_BC1E_7AFA7F1A65F5__INCLUDED_

#include "Plane.h"
#include "Extent.h"
#include "PolygonArray.h"
#include "MyExtensions.h"

namespace OpenGLVolumeRendering {

	/** The base class for all volume renderers. */
	class RendererBase  
	{
	public:
		RendererBase();
		virtual ~RendererBase();

		/// Initializes the renderer.  Should be called again if the renderer is
		/// moved to a different openGL context.  If this returns false, do not try
		/// to use it to do volumeRendering
		virtual bool initRenderer();

		/// Makes the check necessary to determine if this renderer is 
		/// compatible with the hardware its running on
		virtual bool checkCompatibility() const = 0;
		
		/// Sets the aspect ratio of the dataset.
		bool setAspectRatio(double ratioX, double ratioY, double ratioZ);
		/// Specifies the portion of the uploaded texture that should be rendered.
		/// The extenst should range from 0 to 1.
		bool setTextureSubCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
		/// Specifies that we are rendering a subportion of the full data.
		/// Used for out of core rendering.  The extents should range from 0 to 1.
		bool setDataSubVolume(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
		/// Used for out of core rendering.  The dimensions of the full dataset
		bool setHintDimensions(unsigned int hintDimX, unsigned int hintDimY, unsigned int hintDimZ);

		/// Quality is a number from 0 to 1.  Lower means faster.
		bool setQuality(double quality);
		double getQuality() const;

		/// nearPlane is a number from 0 to 1.  0 means no clipping takes place.
		/// 1 means the entire volume is clipped.
		bool setNearPlane(double nearPlane);
		double getNearPlane();

		/// Returns the number of planes rendered in the last call to 
		/// renderVolume.
		int getNumberOfPlanesRendered() const;

		/// Performs the actual rendering.
		virtual bool renderVolume() = 0;

	protected:
		// data

		/// stores the polygons needed to render the volume
		PolygonArray m_PolygonArray;

		/// the number of planes rendered in the last call to renderVolume
		int m_NumberOfPlanesRendered;

		/// a flag which specifies if the renderer has been initialized
		bool m_BaseInitialized;

		/// Specifies the portion of the uploaded texture that should be rendered.
		/// The extenst should range from 0 to 1.
		Extent m_TextureSubCubeExtent;
		/// Specifies that we are rendering a subportion of the full data.
		/// Used for out of core rendering.  The extents should range from 0 to 1.
		Extent m_DataSubCubeExtent;

		/// The aspect ratio of the dataset
		double m_RatioX, m_RatioY, m_RatioZ;
		/// Used for out of core rendering.  The dimensions of the full dataset
		unsigned int m_HintDimX, m_HintDimY, m_HintDimZ;

		/// Quality is a number from 0 to 1.  Lower means faster.
		double m_Quality;
		/// nearPlane is a number from 0 to 1.  0 means no clipping takes place.
		/// 1 means the entire volume is clipped.
		double m_NearPlane;

		/// Vertex array
		float* m_VertexArray;
		/// Texture coordinate array
		float* m_TextureArray;

		/// Vertex array size
		unsigned int m_VertexArraySize;

		/// Actual number of vertices in array
		unsigned int m_NumVertices;

		/// Triangle array
		unsigned int* m_TriangleArray;

		/// Triangle array size
		unsigned int m_TriangleArraySize;

		/// Actual number of triangles in array
		unsigned int m_NumTriangles;

		/// Allocates memory for the vertices and triangles
		bool allocateMemory(unsigned int numVerts, unsigned int numTriangles);

		/// Deallocates the memory for vertices and triangles
		void deallocateMemory();

		/// Allocate the vertex array
		bool allocateVertexArray(unsigned int numVerts);

		/// Allocate the triangle array
		bool allocateTriangleArray(unsigned int numTriangles);

		/// Converts the polygon array to traingle and vertex arrays
		void convertToTriangles();

		/// Sets the aspectRatio to a default value.
		bool initAspectRatio();
		/// Sets all flags to default values.
		bool initFlags();

		/// Returns a plane parallel to the view plane.
		Plane getViewPlane();

		/// Returns the distance between planes.
		double getIntervalWidth() const;
		/// Returns a distance that is past the entire volume.
		double getFurthestDistance() const;
		/// Returns a distance that is before the entire volume.
		double getNearestDistance() const;

		/// Computes the polygons that need to be rendered
		virtual void computePolygons();

	};

};

#endif // !defined(AFX_OPENGLVOLUMERENDERERBASE_H__71ABAABC_1CAE_4AE1_BC1E_7AFA7F1A65F5__INCLUDED_)
