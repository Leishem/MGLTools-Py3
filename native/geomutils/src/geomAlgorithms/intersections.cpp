// Intersections of Lines, Segments  and Planes (2D and 3D)

// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;}
//        operators for:
//            == to test equality
//            != to test inequality
//            Point  = Point ± Vector
//            Vector = Point - Point
//            Vector = Scalar * Vector    (scalar product)
//            Vector = Vector * Vector    (3D cross product)
//    Line and Ray and Segment with defining points {Point P0, P1;}
//        (a Line is infinite, Rays and Segments start at P0)
//        (a Ray extends beyond P1, but a Segment ends at P1)
//    Plane with a point and a normal {Point V0; Vector n;}
//===================================================================

#include "intersections.h"

// intersect2D_2Segments(): the intersection of 2 finite 2D segments
//    Input:  two finite segments S1 and S2
//    Output: *I0 = intersect point (when it exists)
//            *I1 = endpoint of intersect segment [I0,I1] (when it exists)
//    Return: 0=disjoint (no intersect)
//            1=intersect in unique point I0
//            2=overlap in segment from I0 to I1
int intersect2D_Segments( Segment S1, Segment S2, Point* I0, Point* I1 )
{
    Vector    u = S1.P1 - S1.P0;
    Vector    v = S2.P1 - S2.P0;
    Vector    w = S1.P0 - S2.P0;
    float     D = perp(u,v);

    // test if they are parallel (includes either being a point)
    if (fabs(D) < SMALL_NUM) {          // S1 and S2 are parallel
        if (perp(u,w) != 0 || perp(v,w) != 0) {
            return 0;                   // they are NOT collinear
        }
        // they are collinear or degenerate
        // check if they are degenerate points
        float du = dot(u,u);
        float dv = dot(v,v);
        if (du==0 && dv==0) {           // both segments are points
            if (S1.P0 != S2.P0)         // they are distinct points
                return 0;
            *I0 = S1.P0;                // they are the same point
            return 1;
        }
        if (du==0) {                    // S1 is a single point
            if (inSegment(S1.P0, S2) == 0)  // but is not in S2
                return 0;
            *I0 = S1.P0;
            return 1;
        }
        if (dv==0) {                    // S2 a single point
            if (inSegment(S2.P0, S1) == 0)  // but is not in S1
                return 0;
            *I0 = S2.P0;
            return 1;
        }
        // they are collinear segments - get overlap (or not)
        float t0, t1;                   // endpoints of S1 in eqn for S2
        Vector w2 = S1.P1 - S2.P0;
        if (v.x != 0) {
                t0 = w.x / v.x;
                t1 = w2.x / v.x;
        }
        else {
                t0 = w.y / v.y;
                t1 = w2.y / v.y;
        }
        if (t0 > t1) {                  // must have t0 smaller than t1
                float t=t0; t0=t1; t1=t;    // swap if not
        }
        if (t0 > 1 || t1 < 0) {
            return 0;     // NO overlap
        }
        t0 = t0<0? 0 : t0;              // clip to min 0
        t1 = t1>1? 1 : t1;              // clip to max 1
        if (t0 == t1) {                 // intersect is a point
            *I0 = S2.P0 + t0 * v;
            return 1;
        }

        // they overlap in a valid subsegment
        *I0 = S2.P0 + t0 * v;
        *I1 = S2.P0 + t1 * v;
        return 2;
    }

    // the segments are skew and may intersect in a point
    // get the intersect parameter for S1
    float     sI = perp(v,w) / D;
    if (sI < 0 || sI > 1)               // no intersect with S1
        return 0;

    // get the intersect parameter for S2
    float     tI = perp(u,w) / D;
    if (tI < 0 || tI > 1)               // no intersect with S2
        return 0;

    *I0 = S1.P0 + sI * u;               // compute S1 intersect point
    return 1;
}
//===================================================================

// inSegment(): determine if a point is inside a segment
//    Input:  a point P, and a collinear segment S
//    Return: 1 = P is inside S
//            0 = P is not inside S
int inSegment( Point P, Segment S)
{
    if (S.P0.x != S.P1.x) {    // S is not vertical
        if (S.P0.x <= P.x && P.x <= S.P1.x)
            return 1;
        if (S.P0.x >= P.x && P.x >= S.P1.x)
            return 1;
    }
    else {    // S is vertical, so test y coordinate
        if (S.P0.y <= P.y && P.y <= S.P1.y)
            return 1;
        if (S.P0.y >= P.y && P.y >= S.P1.y)
            return 1;
    }
    return 0;
}
//===================================================================

// intersect3D_SegmentPlane(): intersect a segment and a plane
//    Input:  S = a segment, and Pn = a plane = {Point V0; Vector n;}
//    Output: *I0 = the intersect point (when it exists)
//    Return: 0 = disjoint (no intersection)
//            1 = intersection in the unique point *I0
//            2 = the segment lies in the plane
int intersect3D_SegmentPlane( Segment S, Plane Pn, Point* I )
{
    Vector    u = S.P1 - S.P0;
    Vector    w = S.P0 - Pn.V0;

    float     D = dot(Pn.n, u);
    float     N = -dot(Pn.n, w);

    if (fabs(D) < SMALL_NUM) {          // segment is parallel to plane
        if (N == 0)                     // segment lies in plane
            return 2;
        else
            return 0;                   // no intersection
    }
    // they are not parallel
    // compute intersect param
    float sI = N / D;
    if (sI < 0 || sI > 1)
        return 0;                       // no intersection

    *I = S.P0 + sI * u;                 // compute segment intersect point
    return 1;
}
//===================================================================

// intersect3D_2Planes(): the 3D intersect of two planes
//    Input:  two planes Pn1 and Pn2
//    Output: *L = the intersection line (when it exists)
//    Return: 0 = disjoint (no intersection)
//            1 = the two planes coincide
//            2 = intersection in the unique line *L
int intersect3D_2Planes( Plane Pn1, Plane Pn2, Line* L )
{
    Vector   u = Pn1.n * Pn2.n;         // cross product
    float    ax = (u.x >= 0 ? u.x : -u.x);
    float    ay = (u.y >= 0 ? u.y : -u.y);
    float    az = (u.z >= 0 ? u.z : -u.z);

    // test if the two planes are parallel
    if ((ax+ay+az) < SMALL_NUM) {       // Pn1 and Pn2 are near parallel
        // test if disjoint or coincide
        Vector   v = Pn2.V0 - Pn1.V0;
        if (dot(Pn1.n, v) == 0)         // Pn2.V0 lies in Pn1
            return 1;                   // Pn1 and Pn2 coincide
        else
            return 0;                   // Pn1 and Pn2 are disjoint
    }

    // Pn1 and Pn2 intersect in a line
    // first determine max abs coordinate of cross product
    int      maxc;                      // max coordinate
    if (ax > ay) {
        if (ax > az)
             maxc = 1;
        else maxc = 3;
    }
    else {
        if (ay > az)
             maxc = 2;
        else maxc = 3;
    }

    // next, to get a point on the intersect line
    // zero the max coord, and solve for the other two
    Point    iP;               // intersect point
    float    d1, d2;           // the constants in the 2 plane equations
    d1 = -dot(Pn1.n, Pn1.V0);  // note: could be pre-stored with plane
    d2 = -dot(Pn2.n, Pn2.V0);  // ditto

    switch (maxc) {            // select max coordinate
    case 1:                    // intersect with x=0
        iP.x = 0;
        iP.y = (d2*Pn1.n.z - d1*Pn2.n.z) / u.x;
        iP.z = (d1*Pn2.n.y - d2*Pn1.n.y) / u.x;
        break;
    case 2:                    // intersect with y=0
        iP.x = (d1*Pn2.n.z - d2*Pn1.n.z) / u.y;
        iP.y = 0;
        iP.z = (d2*Pn1.n.x - d1*Pn2.n.x) / u.y;
        break;
    case 3:                    // intersect with z=0
        iP.x = (d2*Pn1.n.y - d1*Pn2.n.y) / u.z;
        iP.y = (d1*Pn2.n.x - d2*Pn1.n.x) / u.z;
        iP.z = 0;
    }
    L->P0 = iP;
    L->P1 = iP + u;
    return 2;
}
//===================================================================

// Intersections of Rays, Segments, Planes and Triangles in 3D


// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;}
//        operators for:
//            == to test equality
//            != to test inequality
//            (Vector)0 = (0,0,0)         (null vector)
//            Point  = Point ± Vector
//            Vector = Point - Point
//            Vector = Scalar * Vector    (scalar product)
//            Vector = Vector * Vector    (cross product)
//    Line and Ray and Segment with defining points {Point P0, P1;}
//        (a Line is infinite, Rays and Segments start at P0)
//        (a Ray extends beyond P1, but a Segment ends at P1)
//    Plane with a point and a normal {Point V0; Vector n;}
//    Triangle with defining vertices {Point V0, V1, V2;}
//    Polyline and Polygon with n vertices {int n; Point *V;}
//        (a Polygon has V[n]=V[0])
//===================================================================


// intersect_RayTriangle(): intersect a ray with a 3D triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 = disjoint (no intersect)
//             1 = intersect in unique point I1
//             2 = are in the same plane
int intersect_RayTriangle( Ray R, Triangle T, Point* I )
{
    Vector    u, v, n;             // triangle vectors
    Vector    dir, w0, w;          // ray vectors
    float     r, a, b;             // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    
    u = T.V1 - T.V0;
    v = T.V2 - T.V0;
    n = u ^ v;             // cross product

    if (n == (Vector)0)            // triangle is degenerate
        return -1;                 // do not deal with this case

    dir = R.P1 - R.P0;             // ray direction vector
    w0 = R.P0 - T.V0;
    a = -dot(n,w0);
    b = dot(n,dir);

    if (fabs(b) < SMALL_NUM) {     // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else return 0;             // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)                   // ray goes away from triangle
        return 0;                  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect

    *I = R.P0 + r * dir;           // intersect point of ray and plane
    //printf ("I: (%f, %f, %f) \n", I->x, I->y, I->z);
    // is I inside T?
    float    uu, uv, vv, wu, wv, D;
    uu = dot(u,u);
    uv = dot(u,v);
    vv = dot(v,v);
    w = *I - T.V0;
    wu = dot(w,u);
    wv = dot(w,v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)        // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;
    //printf ("I is in T: (%f, %f, %f) \n", I->x, I->y, I->z);
    //printf ("T:[[%f, %f, %f],[%f,%f,%f],[%f,%f,%f]]\n", T.V0.x, T.V0.y, T.V0.z, T.V1.x, T.V1.y, T.V1.z, T.V2.x, T.V2.y, T.V2.z);
     
    return 1;                      // I is in T
}

/*
rays = [
[[10.082968845963478, 54.963155567646027, 20.209468066692352], 
 [9.9849128723144531, 54.28094482421875, 19.484897613525391]]
    ]

t = [[9.6853628158569336, 54.199100494384766, 19.562088012695312], 
     [10.186515808105469, 54.1707763671875, 19.562088012695312], 
     [10.186515808105469, 54.432743072509766, 19.302522659301758]]

intersect_RayTriangle(rays[0], t)

#right answer
(1, (9.9834707538105185, 54.270911486115345, 19.474241287153518))
*/
