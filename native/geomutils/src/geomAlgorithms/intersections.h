#ifndef SS_Intersections_H
#define SS_Intersections_H

#include "geomstructs.h"

// intersect2D_2Segments(): the intersection of 2 finite 2D segments
//    Input:  two finite segments S1 and S2
//    Output: *I0 = intersect point (when it exists)
//            *I1 = endpoint of intersect segment [I0,I1] (when it exists)
//    Return: 0=disjoint (no intersect)
//            1=intersect in unique point I0
//            2=overlap in segment from I0 to I1
int intersect2D_Segments( Segment S1, Segment S2, Point* I0, Point* I1 );

// inSegment(): determine if a point is inside a segment
//    Input:  a point P, and a collinear segment S
//    Return: 1 = P is inside S
//            0 = P is not inside S
int inSegment( Point P, Segment S);

// intersect3D_SegmentPlane(): intersect a segment and a plane
//    Input:  S = a segment, and Pn = a plane = {Point V0; Vector n;}
//    Output: *I0 = the intersect point (when it exists)
//    Return: 0 = disjoint (no intersection)
//            1 = intersection in the unique point *I0
//            2 = the segment lies in the plane
int intersect3D_SegmentPlane( Segment S, Plane Pn, Point* I );

// intersect3D_2Planes(): the 3D intersect of two planes
//    Input:  two planes Pn1 and Pn2
//    Output: *L = the intersection line (when it exists)
//    Return: 0 = disjoint (no intersection)
//            1 = the two planes coincide
//            2 = intersection in the unique line *L
int intersect3D_2Planes( Plane Pn1, Plane Pn2, Line* L );

// intersect_RayTriangle(): intersect a ray with a 3D triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 = disjoint (no intersect)
//             1 = intersect in unique point I1
//             2 = are in the same plane
int intersect_RayTriangle( Ray R, Triangle T, Point* I );


#endif
