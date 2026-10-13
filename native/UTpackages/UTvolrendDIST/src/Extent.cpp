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

// Extent.cpp: implementation of the Extent class.
//
//////////////////////////////////////////////////////////////////////

#include "Extent.h"

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Extent::Extent()
{
	setExtents(0, 0, 0, 1, 1, 1);
}

Extent::Extent(
									   double xMin, double yMin, double zMin,
									   double xMax, double yMax, double zMax
									   )
{
	setExtents(xMin, yMin, zMin, xMax, yMax, zMax);
}

Extent::~Extent()
{

}

void Extent::setExtents(
									double minX, double minY, double minZ,
									double maxX, double maxY, double maxZ
									)
{
	m_MinX = minX; m_MaxX = maxX;
	m_MinY = minY; m_MaxY = maxY;
	m_MinZ = minZ; m_MaxZ = maxZ;
}
