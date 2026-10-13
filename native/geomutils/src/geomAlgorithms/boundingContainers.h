#ifndef _BoundingContainers_H
#define _BoundingContainers_H

#include "geomstructs.h"

// fastBall(): a fast approximation of the bounding ball for a point set
//               based on the algorithm given by [Jack Ritter, 1990]
//    Input:  an array V[] of n points
//    Output: a bounding ball = {Point center; float radius;}
void fastBall( Point V[], int n, Ball* B);

#endif
