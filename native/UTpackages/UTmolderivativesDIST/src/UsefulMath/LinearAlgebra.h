/*****************************************************************************/
/*                                                                           */
/*   UsefulMath: Some little math routines which dont fit anywhere else.     */
/*                                                                           */
/*   Copyright (C) The University of Texas at Austin                         */
/*                                                                           */
/*     Author:     Vinay Siddavanahalli <skvinay@cs.utexas.edu> 2004-2005    */
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
// LinearAlgebra.h: interface for the LinearAlgebra class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEARALGEBRA_H__AD644F94_D742_4858_A926_AC7E6964B1E2__INCLUDED_)
#define AFX_LINEARALGEBRA_H__AD644F94_D742_4858_A926_AC7E6964B1E2__INCLUDED_

#include "Vector.h"

namespace CCVOpenGLMath {

	class LinearAlgebra  
	{
	public:
		LinearAlgebra();
		virtual ~LinearAlgebra();

		static bool getCylinderFit( int n, double* x, double* y, double* z, CCVOpenGLMath::Vector* p1, CCVOpenGLMath::Vector* p2, double* radius );

		// fit a line y = mx + c minimizing the least squares norm.
		static bool leastSquares( int n, double* x, double* y, double* m, double* c, double* radius );

		static bool mean( double* x, int n, double* mean );
		static bool summation( double* x, int n, double* sum );
		static bool sumOfSquares( double* x, int n, double* sumSquare );
		static bool dotProduct( double* x, double* y, int n, double* dotProd );
		static bool correlate( double* x, double* y, int n, double* correlationCoefficient );
		static bool selectivelyCorrelate( double* x, int rangeToCorrelate, double* y, int n, double* correlationCoefficient );

		static bool discretize( double* x, int n, double posVal, double negVal );
		// solve ax=b
		static bool solveSystem(double a11, double a12, double a13,
								double a21, double a22, double a23,
								double a31, double a32, double a33,
								double b1,  double b2,  double b3,
								double* x,  double* y,  double* z);

		static bool solve2x2System(	double a11, double a12, double b1,
									double a21, double a22, double b2,
									double* x, double* y);
		static bool solveEigenSystem(	double c11, double c12, double c13, 
										double c21, double c22, double c23, 
										double c31, double c32, double c33, 
										double* k1Vec, double* k2Vec, double g1, double g2);
		static bool solveDependentEquations(	double c11, double c12, double c13, 
												double c21, double c22, double c23, 
												double c31, double c32, double c33, 
												double* vec );

	};
};

#endif // !defined(AFX_LINEARALGEBRA_H__AD644F94_D742_4858_A926_AC7E6964B1E2__INCLUDED_)
