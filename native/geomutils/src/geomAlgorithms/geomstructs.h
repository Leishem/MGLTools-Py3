#ifndef SS_Geomstructs_H
#define SS_Geomstructs_H

#include "point.h"
#include "Vector.h"
//    Line, Ray and Segment with defining points {Point P0, P1;}.
//   Line is infinite, Rays and Segments start at P0,
//   a Ray extends beyond P1, but a Segment ends at P1.

typedef struct {
  Point P0, P1;
} Line;

typedef struct {
  Point P0, P1;
} Segment;

typedef struct {
  Point P0, P1;
} Ray;
  
typedef struct {
  Point V0; 
  Vector n;
} Plane;

typedef struct {
  //initial position and velocity vector
  Point P0;   
  Vector v;
}Track;

//    Triangle with defining vertices {Point V0, V1, V2;}

typedef struct {
Point V0, V1, V2;
} Triangle;

//    Polyline and Polygon with n vertices {int n; Point *V;}
//        (a Polygon has V[n]=V[0])


//    Ball with a center and radius 
typedef struct {
  Point center; 
  float radius;
} Ball;

#endif //SS_Geomstructs_H
