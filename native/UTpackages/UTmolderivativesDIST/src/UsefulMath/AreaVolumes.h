// AreaVolumes.h: interface for the AreaVolumes class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCV_AREA_VOLUMES_H
#define CCV_AREA_VOLUMES_H

namespace CCVOpenGLMath {

	class AreaVolumes  
	{
	public:
		AreaVolumes();
		virtual ~AreaVolumes();

		static double getTriangleArea(	double v1x, double v1y, double v1z,
										double v2x, double v2y, double v2z,
										double v3x, double v3y, double v3z);
	};

};

#endif // CCV_AREA_VOLUMES_H
