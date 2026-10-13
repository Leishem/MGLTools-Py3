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

// Polygon.h: interface for the Polygon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYGON_H__84933946_6F6C_4BA9_9FED_C87A6E762B30__INCLUDED_)
#define AFX_POLYGON_H__84933946_6F6C_4BA9_9FED_C87A6E762B30__INCLUDED_

namespace OpenGLVolumeRendering {

	/** Encapsulates a polygon. */
	class Polygon  
	{
	public:
		Polygon(unsigned int numVerts);
		Polygon();
		virtual ~Polygon();

		double* getVert(unsigned int index);
		double* getTexCoord(unsigned int index);

		unsigned int getNumVerts() const;
		void setNumVerts(unsigned int numVerts);

		unsigned int getNumTriangles() const;

		// Used to break the polygon up into triangles
		// Gives the vertex numbers for each vertex of each triangle
		inline unsigned int getVertexForTriangles(unsigned int i) const;

	protected:
		// this is not a general polygon,
		// it can have at most 6 verts
		double m_Verts[6*3];
		double m_TexCoords[6*3];

		unsigned int m_NumVerts;

	};

}

// Used to break the polygon up into triangles
// Gives the vertex numbers for each vertex of each triangle
inline unsigned int OpenGLVolumeRendering::Polygon::getVertexForTriangles(unsigned int i) const
{
	unsigned int vertices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5};
	if (i<12)
		return vertices[i];
	else 
		return 0;
}


#endif // !defined(AFX_POLYGON_H__84933946_6F6C_4BA9_9FED_C87A6E762B30__INCLUDED_)
