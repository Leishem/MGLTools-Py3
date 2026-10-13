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
// SumOfGaussiansCurvature.h: interface for the SumOfGaussiansCurvature class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_SUM_OF_GAUSSIANS_CURVATURE_H
#define CCV_SUM_OF_GAUSSIANS_CURVATURE_H

#include "Curvature.h"

class CurvaturesGridVoxel;

class SumOfGaussiansCurvature : public Curvature  
{
public:
	SumOfGaussiansCurvature( int numberOfGaussians, double* gaussianCenters, int numberOfGridDivisions, double maxFunctionError,  double blobbiness,
							 int numberOfPoints, float* points, double* HandK, double* normals, double* k1Vector, double* k2Vector);
	virtual ~SumOfGaussiansCurvature();

	bool initialize();

protected:

	void getMinMax();

	void getIndices( double x, double y, double z, int *gridx, int *gridy, int* gridz );
	void populateGrid(double overlapExtent);
	void createGrid();

	inline void evalCurvature(	double* phiX,  double* phiY,  double* phiZ, 
								double* phiXX, double* phiYY, double* phiZZ, 
								double* phiXY, double* phiXZ, double* phiYZ, 
								double x,      double y,      double z );
	inline int getIndex( double x, double y, double z );

	inline void evalCurvatureDueToOneAtom( double* phiX, double* phiY, double* phiZ, double* phiXX, 
										   double* phiYY, double* phiZZ,  double* phiXY, double* phiXZ, 
										   double* phiYZ,  double xc, double yc, double zc, double rad, 
										   double x, double y, double z );

	double m_Min[3], m_Max[3];
	CurvaturesGridVoxel* m_CurvaturesGridVoxels;
	int m_NumberOfGaussians;
	double* m_GaussianCenters; 
	int m_NumberOfGridDivisions; 
	double m_MaxFunctionError;
	double m_Blobbiness;
};

#endif 