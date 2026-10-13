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

// Plane.cpp: implementation of the Plane class.
//
//////////////////////////////////////////////////////////////////////

#include "Plane.h"
#include <math.h>

using namespace OpenGLVolumeRendering;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Plane::Plane()
{

}

Plane::Plane(double a, double b, double c, double d)
{
	m_A = a; m_B = b; m_C = c; m_D = d;
}

Plane::~Plane()
{

}

double Plane::signedDistance(double x, double y, double z) const
{
	return m_A*x+m_B*y+m_C*z-m_D;
}

void Plane::normalizeNormal()
{
	double length = sqrt(m_A*m_A+m_B*m_B+m_C*m_C);
	m_A/=length;
	m_B/=length;
	m_C/=length;
	m_D/=length;
}

