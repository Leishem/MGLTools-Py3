/*
 * cubes.h - marching cubes table of cubes and associated tables
 *           this file has been automatically generated
 *           DO NOT EDIT
 * Copyright (c) 1997 Dan Schikore
 */

// $Id: cubes.h,v 1.1.1.1 2004/06/16 19:46:53 annao Exp $

/* table of intersected edges (complete with holes) */
extern u_char cubes[256][14];


/* table of adjacent faces to visit in contour propagation */
extern u_char adjfaces[256][7];


/* table of cube vertices involved in triangulation */
extern u_char cubeverts[256][9];


/* table of cube edges involved in triangulation */
extern u_char cubeedges[256][13];



