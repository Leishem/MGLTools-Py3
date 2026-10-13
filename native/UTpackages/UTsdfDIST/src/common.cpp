/*
  Copyright 2002-2003 The University of Texas at Austin
  
	Authors: Lalit Karlapalem <ckl@ices.utexas.edu>
	Advisor: Chandrajit Bajaj <bajaj@cs.utexas.edu>

  This is a free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

*/


#include <math.h>

#include "common.h"

#include <time.h>
#ifdef _WIN32
         #include <sys/types.h>
         #include <sys/timeb.h>
#else
         #include <sys/time.h>
#endif
 

int isEqual (double one, double two)
{
	if ( (-1*SDFLibrary::TOLERANCE <= (one-two)) && ((one-two) <= SDFLibrary::TOLERANCE) )
		return true;
	return false;
}

int isZero(double num)
{
	if ( (-1*SDFLibrary::TOLERANCE <= num) && (num <= SDFLibrary::TOLERANCE) )
		return true;
	return false;
}

int isNegative(double num)
{
	if (num <0)	return true;
	return false;
}

int isBetween(double one, double two, double num)
{
	if ( ((one<=num) && (num<=two)) || ((isEqual(num, one)) || (isEqual(num, two))) )
		return true;
	return false;
}

int isZero(SDFLibrary::myPoint one)
{
	double val = sqrt(one.x*one.x + one.y*one.y + one.z*one.z);

	if (isZero(val))
		return true;
	return false;
}

int isSame(SDFLibrary::myPoint one, SDFLibrary::myPoint two)
{
	double val = sqrt( (one.x-two.x)*(one.x-two.x) + (one.y-two.y)*(one.y-two.y) + (one.z-two.z)*(one.z-two.z) );

	if (isZero(val))
		return true;
	return false;
}



void _vert2index(int c, int &i, int &j, int &k)
{
	int _left;

	i = c%(SDFLibrary::size+1);

	_left = c/(SDFLibrary::size+1);
	j = _left%(SDFLibrary::size+1);

	_left = _left/(SDFLibrary::size+1);
	k = _left;

	if (i<0) i=0;		
	if (j<0) j=0;			
	if (k<0) k=0;
	if (i>SDFLibrary::size+1) i=SDFLibrary::size+1;	
	if (j>SDFLibrary::size+1) j=SDFLibrary::size+1;		
	if (k>SDFLibrary::size+1) k=SDFLibrary::size+1;
}

int index2vert(int i, int j, int k)
{
	return(k*(SDFLibrary::size+1)*(SDFLibrary::size+1) + j*(SDFLibrary::size+1) + i);
}


double xCoord(int i)
{
	if ((0<=i) && (i<= SDFLibrary::size+1))
		return((double)i);
	return -1;
}

double yCoord(int j)
{
	if ((0<=j) && (j<= SDFLibrary::size+1))
		return((double)j);
	return -1;
}

double zCoord(int k)
{
	if ((0<=k) && (k<= SDFLibrary::size+1))
		return((double)k);
	return -1;
}



//Get the current time in seconds as a double value 
double getTime()
{
	#ifdef _WIN32
		 time_t ltime;
		 _timeb tstruct;
		 time( &ltime );
		 _ftime( &tstruct );
		 return (double) (ltime + 1e-3*(tstruct.millitm));
	#else
		 struct timeval t;
		 gettimeofday( &t, NULL );
		 return (double)(t.tv_sec + 1e-6*t.tv_usec);
	#endif
}