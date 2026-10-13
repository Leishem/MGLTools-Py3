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

// Tuple.cpp: implementation of the Tuple class.
//
//////////////////////////////////////////////////////////////////////

#include "Tuple.h"

using CCVOpenGLMath::Tuple;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Tuple::Tuple(float x, float y, float z, float w)
{
	set(x,y,z,w);
}

Tuple::Tuple()
{
	set(0.0, 0.0, 0.0, 0.0);
}

Tuple::~Tuple()
{

}

Tuple::Tuple(const Tuple& copy)
{
	set(copy);
}

Tuple& Tuple::operator=(const Tuple& copy)
{
	return set(copy);
}

Tuple& Tuple::set(float x, float y, float z, float w)
{
	p[0] = x;
	p[1] = y;
	p[2] = z;
	p[3] = w;
	return *this;
}

Tuple& Tuple::set(float* array)
{
	p[0] = array[0];
	p[1] = array[1];
	p[2] = array[2];
	p[3] = array[3];
	return *this;
}


Tuple& Tuple::set(const Tuple& copy)
{
	if (this!=&copy) {
		p[0] = copy.p[0];
		p[1] = copy.p[1];
		p[2] = copy.p[2];
		p[3] = copy.p[3];
	}
	return *this;
}



float& Tuple::operator[](unsigned int i)
{
	return p[i];
}


const float& Tuple::operator[](unsigned int i) const
{
	return p[i];	
}


