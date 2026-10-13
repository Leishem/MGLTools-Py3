// AreaVolumes.cpp: implementation of the AreaVolumes class.
//
//////////////////////////////////////////////////////////////////////

#include "AreaVolumes.h"
#include <math.h>

using CCVOpenGLMath::AreaVolumes;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AreaVolumes::AreaVolumes()
{

}

AreaVolumes::~AreaVolumes()
{

}

double AreaVolumes::getTriangleArea(	double v1x, double v1y, double v1z,
										double v2x, double v2y, double v2z,
										double v3x, double v3y, double v3z)
{
	// the area of a triangle with lengths a,b,c is given as
	// 1/4 sqrt( (a+b+c) (b+c-a) (c+a-b) (a+b-c) )

	double a = sqrt((v1x-v2x)*(v1x-v2x) + (v1y-v2y)*(v1y-v2y) + (v1z-v2z)*(v1z-v2z) );
	double b = sqrt((v1x-v3x)*(v1x-v3x) + (v1y-v3y)*(v1y-v3y) + (v1z-v3z)*(v1z-v3z) );
	double c = sqrt((v3x-v2x)*(v3x-v2x) + (v3y-v2y)*(v3y-v2y) + (v3z-v2z)*(v3z-v2z) );

	return 1.0 / 4.0 * sqrt( fabs((a+b+c) * (b+c-a) * (c+a-b) * (a+b-c)) );
}
