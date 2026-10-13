//About Lines and  Distance of a Point to a Line (2D & 3D)

// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;} (z=0 for 2D)
//        appropriate operators for:
//            Point  = Point ± Vector
//            Vector = Point - Point
//            Vector = Scalar * Vector
//    Line with defining endpoints {Point P0, P1;}
//    Segment with defining endpoints {Point P0, P1;}
//===================================================================
#include "distance.h"

// closest2D_Point_to_Line(): finds the closest 2D Point to a Line
//    Input:  an array P[] of n points, and a Line L
//    Return: the index i of the Point P[i] closest to L
int closest2D_Point_to_Line( Point P[], int n, Line L)
{
    // Get coefficients of the implicit line equation.
    // Do NOT normalize since scaling by a constant
    // is irrelevant for just comparing distances.
    int i;
    float a = L.P0.y - L.P1.y;
    float b = L.P1.x - L.P0.x;
    float c = L.P0.x * L.P1.y - L.P1.x * L.P0.y;

    // initialize min index and distance to P[0]
    int mi = 0;
    float min = a * P[0].x + b * P[0].y + c;
    if (min < 0) min = -min;    // absolute value

    // loop through Point array testing for min distance to L
    for (i=1; i<n; i++) {
        // just use dist squared (sqrt not needed for comparison)
        float dist = a * P[i].x + b * P[i].y + c;
        if (dist < 0) dist = -dist;   // absolute value
        if (dist < min) {    // this point is closer
            mi = i;          // so have a new minimum
            min = dist;
        }
    }
    return mi;    // the index of the closest Point P[mi]
}
//===================================================================

// dist_Point_to_Line(): get the distance of a point to a line.
//    Input:  a Point P and a Line L (in any dimension)
//    Return: the shortest distance from P to L
float dist_Point_to_Line( Point P, Line L)
{
    Vector v = L.P1 - L.P0;
    Vector w = P - L.P0;

    double c1 = dot(w,v);
    double c2 = dot(v,v);
    double b = c1 / c2;

    Point Pb = L.P0 + b * v;
    return d(P, Pb);
}
//===================================================================

// dist_Point_to_Segment(): get the distance of a point to a segment.
//    Input:  a Point P and a Segment S (in any dimension)
//    Return: the shortest distance from P to S
float dist_Point_to_Segment( Point P, Segment S)
{
    Vector v = S.P1 - S.P0;
    Vector w = P - S.P0;

    double c1 = dot(w,v);
    if ( c1 <= 0 )
        return d(P, S.P0);

    double c2 = dot(v,v);
    if ( c2 <= c1 )
        return d(P, S.P1);

    double b = c1 / c2;
    Point Pb = S.P0 + b * v;
    return d(P, Pb);
}


//About Planes and Distance of a Point to a Plane

// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;}
//        operators for:
//            Point  = Point ± Vector
//            Vector = Point - Point
//            Vector = Scalar * Vector    (scalar product)
//    Plane with a point and a normal {Point V0; Vector n;}
//===================================================================



// pbase_Plane(): get base of perpendicular from point to a plane
//    Input:  P = a 3D point
//            PL = a plane with point V0 and normal n
//    Output: *B = base point on PL of perpendicular from P
//    Return: the distance from P to the plane PL
float pbase_Plane( Point P, Plane PL, Point* B)
{
    float    sb, sn, sd;

    sn = -dot( PL.n, (P - PL.V0));
    sd = dot(PL.n, PL.n);
    sb = sn / sd;

    *B = P + sb * PL.n;
    return d(P, *B);
}
//===================================================================


//Distance between Lines and Segments with their Closest Point of Approach

// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;}
//        operators for:
//            Point  = Point ± Vector
//            Vector = Point - Point
//            Vector = Vector ± Vector
//            Vector = Scalar * Vector
//    Line and Segment with defining points {Point P0, P1;}
//    Track with initial position and velocity vector
//            {Point P0; Vector v;}
//===================================================================



// dist3D_Line_to_Line():
//    Input:  two 3D lines L1 and L2
//    Return: the shortest distance between L1 and L2
float
dist3D_Line_to_Line( Line L1, Line L2)
{
    Vector   u = L1.P1 - L1.P0;
    Vector   v = L2.P1 - L2.P0;
    Vector   w = L1.P0 - L2.P0;
    float    a = dot(u,u);        // always >= 0
    float    b = dot(u,v);
    float    c = dot(v,v);        // always >= 0
    float    d = dot(u,w);
    float    e = dot(v,w);
    float    D = a*c - b*b;       // always >= 0
    float    sc, tc;

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) {         // the lines are almost parallel
        sc = 0.0;
        tc = (b>c ? d/b : e/c);   // use the largest denominator
    }
    else {
        sc = (b*e - c*d) / D;
        tc = (a*e - b*d) / D;
    }

    // get the difference of the two closest points
    Vector   dP = w + (sc * u) - (tc * v);  // = L1(sc) - L2(tc)

    return norm(dP);   // return the closest distance
}
//===================================================================

// dist3D_Segment_to_Segment():
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
float
dist3D_Segment_to_Segment( Segment S1, Segment S2)
{
    Vector   u = S1.P1 - S1.P0;
    Vector   v = S2.P1 - S2.P0;
    Vector   w = S1.P0 - S2.P0;
    float    a = dot(u,u);        // always >= 0
    float    b = dot(u,v);
    float    c = dot(v,v);        // always >= 0
    float    d = dot(u,w);
    float    e = dot(v,w);
    float    D = a*c - b*b;       // always >= 0
    float    sc, sN, sD = D;      // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;      // tc = tN / tD, default tD = D >= 0
    float    asN, atN;

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) { // the lines are almost parallel
        sN = 0.0;        // force using point P0 on segment S1
        sD = 1.0;        // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) {       // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1 => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {           // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1 => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    //sc = (abs(sN) < SMALL_NUM ? 0.0 : sN / sD); // A.O.commented out, caused syntax error 
    //tc = (abs(tN) < SMALL_NUM ? 0.0 : tN / tD);
    asN = abs(sN);
    atN = abs(tN);
    sc = (asN < SMALL_NUM ? 0.0 : sN/sD);
    tc = (atN < SMALL_NUM ? 0.0 : tN/tD);

    // get the difference of the two closest points
    Vector   dP = w + (sc * u) - (tc * v);  // = S1(sc) - S2(tc)

    return norm(dP);   // return the closest distance
}
//===================================================================

// cpa_time(): compute the time of CPA for two tracks
//    Input:  two tracks Tr1 and Tr2
//    Return: the time at which the two tracks are closest
float
cpa_time( Track Tr1, Track Tr2 )
{
    Vector   dv = Tr1.v - Tr2.v;

    float    dv2 = dot(dv,dv);
    if (dv2 < SMALL_NUM)      // the tracks are almost parallel
        return 0.0;            // any time is ok.  Use time 0.

    Vector   w0 = Tr1.P0 - Tr2.P0;
    float    cpatime = -dot(w0,dv) / dv2;

    return cpatime;            // time of CPA
}
//===================================================================

// cpa_distance(): compute the distance at CPA for two tracks
//    Input:  two tracks Tr1 and Tr2
//    Return: the distance for which the two tracks are closest
float
cpa_distance( Track Tr1, Track Tr2 )
{
    float    ctime = cpa_time( Tr1, Tr2);
    Point    P1 = Tr1.P0 + (ctime * Tr1.v);
    Point    P2 = Tr2.P0 + (ctime * Tr2.v);

    return d(P1,P2);           // distance at CPA
}
//===================================================================

