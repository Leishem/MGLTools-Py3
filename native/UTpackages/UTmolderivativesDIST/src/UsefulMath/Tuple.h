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

// Tuple.h: interface for the Tuple class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TUPLE_H__5AD4C604_B71A_4924_941A_15A0955C4E4E__INCLUDED_)
#define AFX_TUPLE_H__5AD4C604_B71A_4924_941A_15A0955C4E4E__INCLUDED_

namespace CCVOpenGLMath {

class Tuple  
{
public:
	Tuple(float x, float y, float z, float w);
	Tuple();
	virtual ~Tuple();
	Tuple(const Tuple& copy);
	Tuple& operator=(const Tuple& copy);


	Tuple& set(float x, float y, float z, float w);
	Tuple& set(float* array);
	Tuple& set(const Tuple& copy);
	
	float& operator[](unsigned int i);
	const float& operator[](unsigned int i) const;

protected:
	float p[4];

};

};

#endif // !defined(AFX_TUPLE_H__5AD4C604_B71A_4924_941A_15A0955C4E4E__INCLUDED_)
