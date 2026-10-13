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
// Vector.h: interface for the Vector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__C73C6BDB_2D75_4770_B86A_E3329C07A817__INCLUDED_)
#define AFX_VECTOR_H__C73C6BDB_2D75_4770_B86A_E3329C07A817__INCLUDED_

#include "Tuple.h"

namespace CCVOpenGLMath {

class Vector : public Tuple  
{
public:
	Vector(float x, float y, float z, float w);
	Vector(float* array);
	Vector();
	virtual ~Vector();
	Vector(const Vector& copy);
	Vector& operator=(const Vector& copy);

	Vector& set(float x, float y, float z, float w);
	Vector& set(float* array);
	Vector& set(const Vector& copy);

	Vector cross(const Vector& vec) const;
	Vector& crossEquals(const Vector& vec);
	float dot(const Vector& vec) const;

	Vector operator+(const Vector vec) const;
	Vector& operator+=(const Vector vec);
	Vector operator-(const Vector vec) const;
	Vector& operator-=(const Vector vec);

	Vector operator*(float scalar) const;
	Vector& operator*=(float scalar);

	Vector operator-() const;

	Vector& normalize();
	float norm();

	bool isBad();

	static Vector badVector();
	virtual Vector* clone() const;
};

};

#endif // !defined(AFX_VECTOR_H__C73C6BDB_2D75_4770_B86A_E3329C07A817__INCLUDED_)
