/*****************************************************************************/
/*                                                                           */
/*   UsefulMath: Some little math routines which dont fit anywhere else.     */
/*                                                                           */
/*   Copyright (C) The University of Texas at Austin                         */
/*                                                                           */
/*     Authors:     Vinay Siddavanahalli <skvinay@cs.utexas.edu>  2004-2005  */
/*     Authors:     Anthony Thane        <thanea@ices.utexas.edu> 2003-2003  */
/*                                                                           */
/*     Principal Investigator: Chandrajit Bajaj <bajaj@ices.utexas.edu>      */
/*                                                                           */
/*         Professor of Computer Sciences,                                   */
/*         Computational and Applied Mathematics Chair in Visualization,     */
/*         Director, Computational Visualization Center (CVC),               */
/*         Institute of Computational Engineering and Sciences (ICES)        */
/*         The University of Texas at Austin,                                */
/*         201 East 24th Street, ACES 2.324A,                                */
/*         1 University Station, C0200                                       */
/*         Austin, TX 78712-0027                                             */
/*         http://www.cs.utexas.edu/~bajaj                                   */
/*                                                                           */
/*         http://www.ices.utexas.edu/CVC                                    */
/*                                                                           */
/*   This library is free software; you can redistribute it and/or           */
/*   modify it under the terms of the GNU Lesser General Public              */
/*   License as published by the Free Software Foundation; either            */
/*   version 2.1 of the License, or (at your option) any later version.      */
/*   Specifically, this library is free for academic or personal non-profit  */
/*   use, with due acknowledgement. Any or all personal profit / industrial  */
/*   use needs to get a proper license approved from us.                     */
/*                                                                           */
/*   This library is distributed in the hope that it will be useful,         */
/*   but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   Lesser General Public License for more details.                         */
/*                                                                           */
/*   You should have received a copy of the GNU Lesser General Public        */
/*   License along with this library; if not, write to the Free Software     */
/*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307    */
/*   USA                                                                     */
/*                                                                           */
/*****************************************************************************/

// Vector.cpp: implementation of the Vector class.
//
//////////////////////////////////////////////////////////////////////

#include "Vector.h"
#include <math.h>

using CCVOpenGLMath::Tuple;
using CCVOpenGLMath::Vector;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const float EPS = 0.00001f;

Vector::Vector(float x, float y, float z, float w) : Tuple(x,y,z,w)
{
}

Vector::Vector() : Tuple()
{
}

Vector::Vector(float* array)
{
	set(array);
}

Vector::~Vector()
{

}

Vector::Vector(const Vector& copy): Tuple(copy)
{
}

Vector& Vector::operator=(const Vector& copy)
{
	if (this!=&copy) {
		set(copy);
	}
	return *this;
}


Vector& Vector::set(float x, float y, float z, float w)
{
	Tuple::set(x,y,z,w);
	return *this;
}

Vector& Vector::set(float* array)
{
	Tuple::set(array);
	return *this;
}

Vector& Vector::set(const Vector& copy)
{
	Tuple::set(copy);
	return *this;
}

Vector Vector::cross(const Vector& vec) const
{
	return Vector(
		p[1]*vec[2] - p[2]*vec[1],
		p[2]*vec[0] - p[0]*vec[2],
		p[0]*vec[1] - p[1]*vec[0],		
		0.0f		
		);
}

Vector& Vector::crossEquals(const Vector& vec)
{
	return set(
		p[1]*vec[2] - p[2]*vec[1],
		p[2]*vec[0] - p[0]*vec[2],
		p[0]*vec[1] - p[1]*vec[0],		
		0.0f		
		);
}

float Vector::dot(const Vector& vec) const
{
	return p[0]*vec[0] + p[1]*vec[1] + p[2]*vec[2] + p[3]*vec[3]; 
}


Vector Vector::operator+(const Vector vec) const
{
	return Vector(
		p[0]+vec[0],
		p[1]+vec[1],
		p[2]+vec[2],
		p[3]+vec[3]);
}

Vector& Vector::operator+=(const Vector vec)
{
	return set(
		p[0]+vec[0],
		p[1]+vec[1],
		p[2]+vec[2],
		p[3]+vec[3]);
}

Vector Vector::operator-(const Vector vec) const
{
	return Vector(
		p[0]-vec[0],
		p[1]-vec[1],
		p[2]-vec[2],
		p[3]-vec[3]);
}

Vector& Vector::operator-=(const Vector vec)
{
	return set(
		p[0]-vec[0],
		p[1]-vec[1],
		p[2]-vec[2],
		p[3]-vec[3]);
}

Vector Vector::operator*(float scalar) const
{
	return Vector(p[0]*scalar, p[1]*scalar, p[2]*scalar, p[3]);
}

Vector& Vector::operator*=(float scalar)
{
	return set(p[0]*scalar, p[1]*scalar, p[2]*scalar, p[3]);
}

Vector Vector::operator-() const
{
	return Vector(-p[0], -p[1], -p[2], p[3]);
}

Vector& Vector::normalize()
{
	if ((float)fabs(p[3])<=EPS) {
		float length = (float)sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
		return set(p[0]/length,p[1]/length,p[2]/length,0.0f);
	}
	else {
		return set(p[0]/p[3], p[1]/p[3], p[2]/p[3], 1.0f);
	}
}

float Vector::norm()
{
	return (float)sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
}

bool Vector::isBad()
{
	return (p[0]==0.0f && p[1]==0.0f && p[2]==0.0f && p[3]==0.0f);
}

Vector Vector::badVector()
{
	return Vector(0.0f, 0.0f, 0.0f, 0.0f);
}

Vector* Vector::clone() const
{
	return new Vector(*this);
}

