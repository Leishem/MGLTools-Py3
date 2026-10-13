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

// Ray.h: interface for the Ray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAY_H__8760CFFE_A656_460F_A893_4C81FF806264__INCLUDED_)
#define AFX_RAY_H__8760CFFE_A656_460F_A893_4C81FF806264__INCLUDED_

#include "Vector.h"

namespace CCVOpenGLMath {

	class Ray  
	{
	public:
		Ray();
		Ray(const Vector& origin, const Vector& dir);
		virtual ~Ray();

		Vector getPointOnRay(float t) const;

		float nearestTOnXAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;
		float nearestTOnYAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;
		float nearestTOnZAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;

		Vector nearestPointOnXAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;
		Vector nearestPointOnYAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;
		Vector nearestPointOnZAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;

		float distanceToXAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;
		float distanceToYAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;
		float distanceToZAxis(Vector Origin = Vector(0.0, 0.0, 0.0, 1.0)) const;

		bool intersectSphere( Vector center, float radius, Vector *point1, Vector* point2, float *distance1, float* distance2 );

		Vector m_Origin;
		Vector m_Dir;

	};

};

#endif // !defined(AFX_RAY_H__8760CFFE_A656_460F_A893_4C81FF806264__INCLUDED_)
