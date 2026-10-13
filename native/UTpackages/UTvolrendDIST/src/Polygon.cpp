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

// Polygon.cpp: implementation of the Polygon class.
//
//////////////////////////////////////////////////////////////////////

#include "Polygon.h"

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Polygon::Polygon(unsigned int numVerts)
{
	setNumVerts(numVerts);
}

Polygon::Polygon()
{
	setNumVerts(0);
}

Polygon::~Polygon()
{

}

double* Polygon::getVert(unsigned int index)
{
	return &(m_Verts[index*3]);
}

double* Polygon::getTexCoord(unsigned int index)
{
	return &(m_TexCoords[index*3]);
}


unsigned int Polygon::getNumVerts() const
{
	return m_NumVerts;
}

void Polygon::setNumVerts(unsigned int numVerts)
{
	m_NumVerts = (numVerts<=6 ? numVerts : 6);
}

unsigned int Polygon::getNumTriangles() const
{
	return m_NumVerts-2;
}


