// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y;}
//        operators for:
//            Point  = Point ± Vector
//            Vector = Point - Point
//            Vector = Vector ± Vector
//            Vector = Scalar * Vector    (scalar product)
//            Vector = Vector / Scalar    (scalar division)
//    Ball with a center and radius {Point center; float radius;}
//===================================================================

#include "boundingContainers.h"

// fastBall(): a fast approximation of the bounding ball for a point set
//               based on the algorithm given by [Jack Ritter, 1990]
//    Input:  an array V[] of n points
//    Output: a bounding ball = {Point center; float radius;}
void
fastBall( Point V[], int n, Ball* B)
{
    Point C;                           // Center of ball
    float rad, rad2;                   // radius and radius squared
    float xmin, xmax, ymin, ymax;      // bounding box extremes
    int   Pxmin, Pxmax, Pymin, Pymax; // index of V[] at box extreme
    int i;

    // find a large diameter to start with
    // first get the bounding box and V[] extreme points for it
    xmin = xmax = V[0].x;
    ymin = ymax = V[0].y;
    Pxmin = Pxmax = Pymin = Pymax = 0;
    for (i=1; i<n; i++) {
        if (V[i].x < xmin) {
            xmin = V[i].x;
            Pxmin = i;
        }
        else if (V[i].x > xmax) {
            xmax = V[i].x;
            Pxmax = i;
        }
        if (V[i].y < ymin) {
            ymin = V[i].y;
            Pymin = i;
        }
        else if (V[i].y > ymax) {
            ymax = V[i].y;
            Pymax = i;
        }
    }
    // select the largest extent as an initial diameter for the ball
    Vector dVx = V[Pxmax] - V[Pxmin]; // diff of Vx max and min
    Vector dVy = V[Pymax] - V[Pymin]; // diff of Vy max and min
    float dx2 = norm2(dVx); // Vx diff squared
    float dy2 = norm2(dVy); // Vy diff squared
    if (dx2 >= dy2) {                     // x direction is largest extent
        C = V[Pxmin] + (dVx / 2.0);         // Center = midpoint of extremes
        rad2 = norm2(V[Pxmax] - C);         // radius squared
    }
    else {                                // y direction is largest extent
        C = V[Pymin] + (dVy / 2.0);         // Center = midpoint of extremes
        rad2 = norm2(V[Pymax] - C);         // radius squared
    }
    rad = sqrt(rad2);

    // now check that all points V[i] are in the ball
    // and if not, expand the ball just enough to include them
    Vector dV;
    float dist, dist2;
    for (i=0; i<n; i++) {
        dV = V[i] - C;
        dist2 = norm2(dV);
        if (dist2 <= rad2)    // V[i] is inside the ball already
            continue;
        // V[i] not in ball, so expand ball to include it
        dist = sqrt(dist2);
        rad = (rad + dist) / 2.0;         // enlarge radius just enough
        rad2 = rad * rad;
        C = C + ((dist-rad)/dist) * dV;   // shift Center toward V[i]
    }
    B->center = C;
    B->radius = rad;
    return;
}

