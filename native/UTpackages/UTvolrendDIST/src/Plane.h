/*
  Copyright 2002-2003 The University of Texas at Austin
  
	Authors: Anthony Thane <thanea@ices.utexas.edu>
	Advisor: Chandrajit Bajaj <bajaj@cs.utexas.edu>

  This file is part of Volume Rover.

  Volume Rover is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Volume Rover is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Volume Rover; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// Plane.h: interface for the Plane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANE_H__04F216E9_22AB_4373_9CF5_754D66B0F950__INCLUDED_)
#define AFX_PLANE_H__04F216E9_22AB_4373_9CF5_754D66B0F950__INCLUDED_

namespace OpenGLVolumeRendering {

	/** Encapsulates a plane */
	class Plane  
	{
	public:
		Plane();
		Plane(double a, double b, double c, double d);
		virtual ~Plane();

		double signedDistance(double x, double y, double z) const;

		void normalizeNormal();

		inline double& operator[](unsigned int index);
		inline const double& operator[](unsigned int index) const;

		inline double& a();
		inline double& b();
		inline double& c();
		inline double& d();

		inline const double& a() const;
		inline const double& b() const;
		inline const double& c() const;
		inline const double& d() const;

	protected:
		double m_A, m_B, m_C, m_D;

	};

};


inline double& OpenGLVolumeRendering::Plane::operator[](unsigned int index)
{
	if (index == 0) {
		return m_A;
	}
	else if (index == 1) {
		return m_B;
	}
	else if (index == 2) {
		return m_C;
	}
	else {
		return m_D;
	}
}

inline const double& OpenGLVolumeRendering::Plane::operator[](unsigned int index) const
{
	if (index == 0) {
		return m_A;
	}
	else if (index == 1) {
		return m_B;
	}
	else if (index == 2) {
		return m_C;
	}
	else {
		return m_D;
	}
}


inline double& OpenGLVolumeRendering::Plane::a()
{
	return m_A;
}

inline double& OpenGLVolumeRendering::Plane::b()
{
	return m_B;
}

inline double& OpenGLVolumeRendering::Plane::c()
{
	return m_C;
}

inline double& OpenGLVolumeRendering::Plane::d()
{
	return m_D;
}


inline const double& OpenGLVolumeRendering::Plane::a() const
{
	return m_A;
}

inline const double& OpenGLVolumeRendering::Plane::b() const
{
	return m_B;
}

inline const double& OpenGLVolumeRendering::Plane::c() const
{
	return m_C;
}

inline const double& OpenGLVolumeRendering::Plane::d() const
{
	return m_D;
}



#endif // !defined(AFX_OPENGLVOLUMEPLANE_H__04F216E9_22AB_4373_9CF5_754D66B0F950__INCLUDED_)

