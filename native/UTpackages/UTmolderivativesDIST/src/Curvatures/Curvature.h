/*****************************************************************************/
/*                                                                           */
/*   Curvatures: Library used to compute derivatives of Sum of Gaussians     */
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
// Curvature.h: interface for the Curvature class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_CURVATURE_H
#define CCV_CURVATURE_H

class Curvature  
{
public:
	Curvature(int numberOfPoints, float* points, double* HandK, double* normals, double* k1Vector, double* k2Vector);
	virtual ~Curvature();

	bool getCurvatures();
	bool write(const char* filename);

	static bool read( const char* filename, int* numberOfPoints, double** HandK, double** normals, double** k1Vector, double** k2Vector);

protected:

	virtual void evalCurvature(	double* phiX,  double* phiY,  double* phiZ, 
								double* phiXX, double* phiYY, double* phiZZ, 
								double* phiXY, double* phiXZ, double* phiYZ, 
								double x,      double y,      double z ) = 0;

	static bool read2Values( FILE* fp, int numberOfPoints, double** values );
	static bool read3Values( FILE* fp, int numberOfPoints, double** values );

	int m_NumberOfPoints;
	float* m_Points;

	double* m_HandK;
	double* m_Normals;
	double* m_K1Vector;
	double* m_K2Vector;

	bool m_Initialized;
};

#endif 

