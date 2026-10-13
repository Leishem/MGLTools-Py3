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

// PolygonArray.h: interface for the PolygonArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYGONARRAY_H__9F01EA71_FDFE_47B4_A507_826834CEF66A__INCLUDED_)
#define AFX_POLYGONARRAY_H__9F01EA71_FDFE_47B4_A507_826834CEF66A__INCLUDED_


namespace OpenGLVolumeRendering {
	class Polygon;

	/** Encapsulates an array of polygons */
	class PolygonArray  
	{
	public:
		PolygonArray(unsigned int sizeGuess);
		virtual ~PolygonArray();

		void clearPolygons();
		void addPolygon(const Polygon& polygon);
		Polygon* getPolygon(unsigned int i);

		unsigned int getNumPolygons();

	protected:
		void doubleArray();

		void allocateArray(unsigned int sizeGuess);

		Polygon* m_PolygonArray;
		unsigned int m_ArraySize;
		unsigned int m_NumPolygons;
	};

};

#endif // !defined(AFX_POLYGONARRAY_H__9F01EA71_FDFE_47B4_A507_826834CEF66A__INCLUDED_)
