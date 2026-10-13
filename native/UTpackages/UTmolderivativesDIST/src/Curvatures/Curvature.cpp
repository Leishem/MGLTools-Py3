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
// Curvature.cpp: implementation of the Curvature class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Curvature.h"
#include "LinearAlgebra.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Curvature::Curvature(int numberOfPoints, float* points, double* HandK, double* normals, double* k1Vector, double* k2Vector )
{
	m_NumberOfPoints = numberOfPoints;
	m_Points = points;

	m_HandK = HandK;
	m_Normals = normals;
	m_K1Vector = k1Vector;
	m_K2Vector = k2Vector;
	
	m_Initialized = false;
}

Curvature::~Curvature()
{

}

bool Curvature::getCurvatures()
{
	if( !m_Initialized ) return false;

	int i;

	fprintf( stderr, "Getting curvatures\n");
	// for each grid g 
	//		for each point p in g
	//			get contribution from each atom in g to p
	//			add contribution to m_HandK at correct index
	//		endfor
	// endfor

	for( i=0; i<m_NumberOfPoints; i++ )
	{
		m_HandK[i*2+0] = m_HandK[i*2+1] = 0;
		double x = m_Points[3*i+0];
		double y = m_Points[3*i+1];
		double z = m_Points[3*i+2];
		
		double phiX = 0;
		double phiY = 0;
		double phiZ = 0;
		
		double phiXX = 0;
		double phiYY = 0;
		double phiZZ = 0;

		double phiXY = 0;
		double phiXZ = 0;
		double phiYZ = 0;

		evalCurvature( &phiX, &phiY, &phiZ, &phiXX, &phiYY, &phiZZ, &phiXY, &phiXZ, &phiYZ, x, y, z );		

		if( (phiX*phiX + phiY*phiY + phiZ*phiZ) < 1e-30 ) 
		{
		//	printf("This is too low, should not happen\n");
			continue;
		}

		// mean curvature
		m_HandK[i*2+0] = ( phiX*phiX*(phiYY+phiZZ) + 
						 phiY*phiY*(phiXX+phiZZ) + 
						 phiZ*phiZ*(phiXX+phiYY) -
						 2*phiX*phiY*phiXY -
						 2*phiY*phiZ*phiYZ-
						 2*phiX*phiZ*phiXZ ) /
					   ( 2*pow(phiX*phiX + phiY*phiY + phiZ*phiZ, 1.5) );

		// gaussian curvature
		m_HandK[i*2+1] = ( phiZ*phiZ*(phiXX*phiYY-phiXY*phiXY) +
						 phiY*phiY*(phiXX*phiZZ-phiXZ*phiXZ) +
						 phiX*phiX*(phiYY*phiZZ-phiYZ*phiYZ) +
						 2*phiX*phiY*(phiXZ*phiYZ - phiXY*phiZZ) +
						 2*phiX*phiZ*(phiXY*phiYZ - phiXZ*phiYY) +
						 2*phiY*phiZ*(phiXY*phiXZ - phiYZ*phiXX) ) /
					   ( pow(phiX*phiX + phiY*phiY + phiZ*phiZ, 2.0) );

		double g = sqrt(phiX*phiX + phiY*phiY + phiZ*phiZ );

		// find k1, k2:
		double H = m_HandK[i*2+0], K = m_HandK[i*2+1];
		double H2minusK = H*H -K;
		if( H2minusK < 0 ) H2minusK = 0;
		double k1 = H - sqrt(H2minusK);
		double k2 = H + sqrt(H2minusK);

		// find normal:
		m_Normals[i*3+0] = phiX / g;
		m_Normals[i*3+1] = phiY / g;
		m_Normals[i*3+2] = phiZ / g;

		double tK1Vector[3];
		double tK2Vector[3];

		tK1Vector[0] = tK1Vector[1] = tK1Vector[2] = tK2Vector[0] = tK2Vector[1] = tK2Vector[2] = 0;
		m_K1Vector[i*3+0] = m_K1Vector[i*3+1] = m_K1Vector[i*3+2] = 0;
		m_K2Vector[i*3+0] = m_K2Vector[i*3+1] = m_K2Vector[i*3+2] = 0;
		if( fabs(k1-k2) > 1e-10 )
		{
			// construct curvature matrix. Find eigenvectors corresponding to k1 and k2.
			double c11, c12, c13;
			double c21, c22, c23;
			double c31, c32, c33;

			double g3 = g*g*g;
			double g2 = g*g;

			c11 = (phiXX*g2 - phiX*(phiX*phiXX + phiY*phiXY + phiZ*phiXZ)) / g3;
			c12 = (phiXY*g2 - phiX*(phiX*phiXY + phiY*phiYY + phiZ*phiYZ)) / g3;
			c13 = (phiXZ*g2 - phiX*(phiX*phiXZ + phiY*phiYZ + phiZ*phiZZ)) / g3;

			c21 = (phiXY*g2 - phiY*(phiX*phiXX + phiY*phiXY + phiZ*phiXZ)) / g3;
			c22 = (phiYY*g2 - phiY*(phiX*phiXY + phiY*phiYY + phiZ*phiYZ)) / g3;
			c23 = (phiYZ*g2 - phiY*(phiX*phiXZ + phiY*phiYZ + phiZ*phiZZ)) / g3;

			c31 = (phiXZ*g2 - phiZ*(phiX*phiXX + phiY*phiXY + phiZ*phiXZ)) / g3;
			c32 = (phiYZ*g2 - phiZ*(phiX*phiXY + phiY*phiYY + phiZ*phiYZ)) / g3;
			c33 = (phiZZ*g2 - phiZ*(phiX*phiXZ + phiY*phiYZ + phiZ*phiZZ)) / g3;

			if( CCVOpenGLMath::LinearAlgebra::solveEigenSystem(c11, c12, c13, c21, c22, c23, c31, c32, c33, tK1Vector, tK2Vector, k1, k2) )
			{
				double d;
				d = sqrt(tK1Vector[0]*tK1Vector[0] + tK1Vector[1]*tK1Vector[1] + tK1Vector[2]*tK1Vector[2]);
				tK1Vector[0] /= d;
				tK1Vector[1] /= d;
				tK1Vector[2] /= d;
				d = sqrt(tK2Vector[0]*tK2Vector[0] + tK2Vector[1]*tK2Vector[1] + tK2Vector[2]*tK2Vector[2]);
				tK2Vector[0] /= d;
				tK2Vector[1] /= d;
				tK2Vector[2] /= d;
				m_K1Vector[i*3+0] = tK1Vector[0]; m_K1Vector[i*3+1] = tK1Vector[1]; m_K1Vector[i*3+2] = tK1Vector[2];
				m_K2Vector[i*3+0] = tK2Vector[0]; m_K2Vector[i*3+1] = tK2Vector[1]; m_K2Vector[i*3+2] = tK2Vector[2];
			}

			// check eigenvalue eigenvectors solution
			{
				double e1 = (c11-k1)*m_K1Vector[i*3+0] + c12*m_K1Vector[i*3+1] + c13*m_K1Vector[i*3+2];
				double e2 = c21*m_K1Vector[i*3+0] + (c22-k1)*m_K1Vector[i*3+1] + c23*m_K1Vector[i*3+2];
				double e3 = c31*m_K1Vector[i*3+0] + c32*m_K1Vector[i*3+1] + (c33-k1)*m_K1Vector[i*3+2];
			}
		}

		if (((i+1) % 20) == 0 || (i+1) == m_NumberOfPoints)
		{
			printf("Evaluating functions %2.2f%% done (%012d)\r", 100.0*(i+1)/(float)(m_NumberOfPoints), i+1);
		}
	}
	printf("\n");

	return true;
}
	

bool Curvature::write(const char* filename)
{
	if( !filename ) return false;
	if( !m_HandK || !m_Normals || !m_K1Vector || !m_K2Vector || (m_NumberOfPoints < 1) ) return false;

	FILE* fp = fopen( filename, "w" );
	if( !fp ) return false;

	fprintf( fp, "#\n");
	fprintf( fp, "# Curvature file exported from TexMol.\n");
	fprintf( fp, "# CVC, The University of Texas at Austin.\n");
	fprintf( fp, "#\n");
	fprintf( fp, "# Format:\n");
	fprintf( fp, "# First see the statement: Number: number of points\n");
	fprintf( fp, "# Then, search for one of the following: HandK, Normal, K1Vector, K2Vector\n");
	fprintf( fp, "# Any of the above should be followed by the corresponding values with 2, 3, 3, 3 in each line\n");
	fprintf( fp, "#\n");
	fprintf( fp, "#(All comments start with '#')\n");
	fprintf( fp, "#\n");
	fprintf(fp, "Number: %d\n", m_NumberOfPoints );
	int i;

	fprintf( fp, "HandK\n");
	for( i=0; i<m_NumberOfPoints; i++ )
		fprintf( fp, "%lf %lf\n", m_HandK[i*2+0], m_HandK[i*2+1] );
	fprintf( fp, "Normal\n");
	for( i=0; i<m_NumberOfPoints; i++ )
		fprintf( fp, "%lf %lf %lf\n", m_Normals[i*3+0], m_Normals[i*3+1], m_Normals[i*3+2] );
	fprintf( fp, "K1Vector\n");
	for( i=0; i<m_NumberOfPoints; i++ )
		fprintf( fp, "%lf %lf %lf\n", m_K1Vector[i*3+0], m_K1Vector[i*3+1], m_K1Vector[i*3+2] );
	fprintf( fp, "K2Vector\n");
	for( i=0; i<m_NumberOfPoints; i++ )
		fprintf( fp, "%lf %lf %lf\n", m_K2Vector[i*3+0], m_K2Vector[i*3+1], m_K2Vector[i*3+2] );
	
	fclose(fp);

	return true;
}

bool Curvature::read2Values( FILE* fp, int numberOfPoints, double** values )
{
	if( !values || (numberOfPoints<1) || !fp )
		return false;

	char line[256];
	memset(line, '\0', sizeof(line));	// Clear out the buffer in case of short lines
	int number = 0;
	while( fgets( line, 255, fp ) != 0 )
	{
		if( sscanf( line, "%lf %lf\n", &((*values)[number*2+0]), &((*values)[number*2+1]) ) != 2 )
			return false;
		number++;
		if( number == numberOfPoints ) return true;
	}
	return true;
}

bool Curvature::read3Values( FILE* fp, int numberOfPoints, double** values )
{
	if( !values || (numberOfPoints<1) || !fp )
		return false;

	char line[256];
	memset(line, '\0', sizeof(line));	// Clear out the buffer in case of short lines
	int number = 0;
	while( fgets( line, 255, fp ) != 0 )
	{
		if( sscanf( line, "%lf %lf %lf\n", &((*values)[number*3+0]), &((*values)[number*3+1]), &((*values)[number*3+2]) ) != 3 )
			return false;
		number++;
		if( number == numberOfPoints ) return true;
	}
	return true;
}

bool Curvature::read( const char* filename, int* numberOfPoints, double** HandK, double** normals, double** k1Vector, double** k2Vector)
{
	if( !filename || !numberOfPoints || !HandK || !normals || !k1Vector || !k2Vector ) return false;

	FILE* fp = fopen( filename, "r" );
	if( !fp ) return false;

	*numberOfPoints = 0; *HandK = 0; *normals = 0; *k1Vector = 0; *k2Vector = 0;

	char line[256];
	memset(line, '\0', sizeof(line));	// Clear out the buffer in case of short lines
	while( fgets( line, 255, fp ) != 0 )
	{
		if( line[0] == '#' ) continue;

		if( (*numberOfPoints) == 0 ) 
		{
			char* occurancePtr = strstr( line, "Number:");
			if( occurancePtr == line )
			{
				char temp[256];
				if( sscanf( line, "%s %d\n", temp, numberOfPoints ) != 2 ) 
				{
					*numberOfPoints = 0;
					fclose( fp );
					return false;
				}
				if( *numberOfPoints < 1 )
				{
					*numberOfPoints = 0;
					fclose( fp );
					return false;
				}
			}
			continue;
		}

		char* occurancePtr = 0;

		if( strstr( line, "HandK") )
		{
			(*HandK) = new double[(*numberOfPoints)*2];
			if( !read2Values( fp, *numberOfPoints, HandK ) )
			{
				delete [](*HandK); *HandK = 0; 
			}
			continue;
		}
		
		if( strstr( line, "Normal") )
		{
			(*normals) = new double[(*numberOfPoints)*3];
			if( !read3Values( fp, *numberOfPoints, normals ) )
			{
				delete [](*normals); *normals = 0; 
			}
			continue;
		}
		if( strstr( line, "K1Vector") )
		{
			(*k1Vector) = new double[(*numberOfPoints)*3];
			if( !read3Values( fp, *numberOfPoints, k1Vector ) )
			{
				delete [](*k1Vector); *k1Vector = 0; 
			}
			continue;
		}
		if( strstr( line, "K2Vector") )
		{
			(*k2Vector) = new double[(*numberOfPoints)*3];
			if( !read3Values( fp, *numberOfPoints, k2Vector ) )
			{
				delete [](*k2Vector); *k2Vector = 0; 
			}
			continue;
		}
	}

	fclose( fp );

	return true;
}
