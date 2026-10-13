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

// PolygonArray.cpp: implementation of the PolygonArray class.
//
//////////////////////////////////////////////////////////////////////

#include "PolygonArray.h"
#include "Polygon.h"

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PolygonArray::PolygonArray(unsigned int sizeGuess)
{
	m_PolygonArray = 0;
	m_ArraySize = 0;
	m_NumPolygons = 0;
	allocateArray(sizeGuess);
}

PolygonArray::~PolygonArray()
{
	delete [] m_PolygonArray;
}

void PolygonArray::clearPolygons()
{
	m_NumPolygons = 0;
}

void PolygonArray::addPolygon(const Polygon& polygon)
{
	doubleArray();
	m_PolygonArray[m_NumPolygons] = polygon;
	m_NumPolygons++;
}

Polygon* PolygonArray::getPolygon(unsigned int i)
{
	if (i<m_NumPolygons) {
		return (&m_PolygonArray[i]);
	}
	else {
		return 0;
	}
}

unsigned int PolygonArray::getNumPolygons()
{
	return m_NumPolygons;
}

void PolygonArray::doubleArray()
{
	if (m_NumPolygons == m_ArraySize) {
		Polygon* temp = new Polygon[m_ArraySize*2];
		if (temp) {
			unsigned int c;
			for (c=0; c<m_NumPolygons; c++) {
				temp[c] = m_PolygonArray[c];
			}
			delete [] m_PolygonArray;
			m_PolygonArray = temp;
			m_ArraySize*=2;
		}
	}
}

void PolygonArray::allocateArray(unsigned int sizeGuess)
{
	delete [] m_PolygonArray;
	m_PolygonArray = new Polygon[sizeGuess];
	m_ArraySize = sizeGuess;
}

