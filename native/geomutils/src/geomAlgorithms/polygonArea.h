#ifndef _PolygonArea_
#define _PolygonArea_

typedef struct {double x, y, z;} Vert;    // exclude z for 2D

// orientation2D_Polygon(): tests the orientation of a simple polygon
//    Input:  int n = the number of vertices in the polygon
//            Vert* V = an array of n+1 vertices with V[n]=V[0]
//    Return: >0 for counterclockwise
//            =0 for none (degenerate)
//            <0 for clockwise
//    Note: this algorithm is faster than computing the signed area.
int orientation2D_Polygon( int n, Vert* V );

// area2D_Polygon(): computes the area of a 2D polygon
//    Input:  int n = the number of vertices in the polygon
//            Vert* V = an array of n+2 vertices
//                       with V[n]=V[0] and V[n+1]=V[1]
//    Return: the (float) area of the polygon
float area2D_Polygon( int n, Vert* V );

// area3D_Polygon(): computes the area of a 3D planar polygon
//    Input:  int n = the number of vertices in the polygon
//            Vert* V = an array of n+2 vertices in a plane
//                       with V[n]=V[0] and V[n+1]=V[1]
//            Vert N = unit normal vector of the polygon's plane
//    Return: the (float) area of the polygon
float area3D_Polygon( int n, Vert* V, Vert N );

#endif
