#ifndef SS_Distance_H
#define SS_Distance_H

#include "geomstructs.h"

// closest2D_Point_to_Line(): finds the closest 2D Point to a Line
//    Input:  an array P[] of n points, and a Line L
//    Return: the index i of the Point P[i] closest to L
int closest2D_Point_to_Line( Point P[], int n, Line L);

// dist_Point_to_Line(): get the distance of a point to a line.
//    Input:  a Point P and a Line L (in any dimension)
//    Return: the shortest distance from P to L
float dist_Point_to_Line( Point P, Line L);

// dist_Point_to_Segment(): get the distance of a point to a segment.
//    Input:  a Point P and a Segment S (in any dimension)
//    Return: the shortest distance from P to S
float dist_Point_to_Segment( Point P, Segment S);


// pbase_Plane(): get base of perpendicular from point to a plane
//    Input:  P = a 3D point
//            PL = a plane with point V0 and normal n
//    Output: *B = base point on PL of perpendicular from P
//    Return: the distance from P to the plane PL
float pbase_Plane( Point P, Plane PL, Point* B);

// dist3D_Line_to_Line():
//    Input:  two 3D lines L1 and L2
//    Return: the shortest distance between L1 and L2
float dist3D_Line_to_Line( Line L1, Line L2);

// dist3D_Segment_to_Segment():
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
float dist3D_Segment_to_Segment( Segment S1, Segment S2);

// cpa_time(): compute the time of CPA for two tracks
//    Input:  two tracks Tr1 and Tr2
//    Return: the time at which the two tracks are closest
float cpa_time( Track Tr1, Track Tr2 );

// cpa_distance(): compute the distance at CPA for two tracks
//    Input:  two tracks Tr1 and Tr2
//    Return: the distance for which the two tracks are closest
float cpa_distance( Track Tr1, Track Tr2 );

#endif //SS_Distance_H
