//------------------------------------------------------------------
// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from it's use.
// Users of this code must verify correctness for their application.
//------------------------------------------------------------------

#ifndef SS_Common_H
#define SS_Common_H

#include<math.h>
#include <iostream>

//enum boolean {FALSE=0, TRUE=1, ERROR=(-1)};
enum softSurferBoolean {FALSE_=0, TRUE_=1, ERROR_=(-1)};
// Error codes
enum Error {
	Enot,	// no error
	Edim,	// error: dim of point invalid for operation
	Esum	// error: sum not affine (cooefs add to 1)
};

// utility macros
#define SMALL_NUM  0.00000001 // anything that avoids division overflow
#define	abs(x)		((x) >= 0 ? x : -(x));
#define	min(x,y)	((x) < (y) ? (x) : (y));
#define	max(x,y)	((x) > (y) ? (x) : (y));

// dot product (3D) which allows vector operations in arguments
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define norm(v)    sqrt(dot(v,v))  // norm = length of vector
#define norm2(v) dot(v,v)
// The distance between points is implemented in point.cpp
//#define d(u,v)     norm(u-v)       // distance = norm of difference
#define perp(u,v)  ((u).x * (v).y - (u).y * (v).x)  // perp product (2D)


#endif //SS_Common_H
