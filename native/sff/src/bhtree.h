/* $Header: /opt/cvs/sffDIST/src/bhtree.h,v 1.1 2004/09/22 19:03:15 annao Exp $
 *
 * $Id: bhtree.h,v 1.1 2004/09/22 19:03:15 annao Exp $
 *
 * $Log: bhtree.h,v $
 * Revision 1.1  2004/09/22 19:03:15  annao
 * -moved files from sffDIST/sff/src into sffDIST/src.
 *
 * Revision 1.1.1.1  2004/06/15 17:10:32  annao
 * Imported sources
 *
 * Revision 1.1.1.1  2002/06/26 23:29:27  gillet
 * Imported sources version1.0
 *
 * Revision 1.1  2001/12/28 01:25:50  sanner
 * - added BHtree-based nblist calculation (turns out to be slower for 4sgb=300AA)
 * - added upper-cutoff in orginial nblist to speedup list genenration
 * - added md_options function
 * - split verbose variable into verbosemm and verbosemd
 * - renamed class Amber94Minimizer Amber94. The run method is now called
 *   minimize and an md method has been added.
 *
 * Revision 0.2  2000/08/14 18:00:28  sanner
 * removed copyright text
 *
 * Revision 0.1  2000/08/14 17:45:35  sanner
 * added copyright text
 *
 * Revision 0.0  1999/10/27 17:51:28  sanner
 * *** empty log message ***
 *
 * Revision 1.3  1998/03/19  22:10:14  sanner
 * fixed the RCS Header Id and Log in source files
 *
 */
/* 
   provided by Armin Widmer
*/
#ifndef BHTREEDEF
#define BHTREEDEF

#include <stdio.h>

typedef struct BHpoint {
  float x[3];
  float r;
  int   at;
} BHpoint;

typedef struct BHnode {
  struct BHnode *left,*right;
  struct BHpoint **atom;
  float  cut;
  int    dim,n;
} BHnode;  

typedef struct BHtree {
  struct BHnode *root;
  struct BHpoint **atom;
  float xmin[3];
  float xmax[3];
  float rm;
#ifdef STATBHTREE
  long tot;    /* total number of neighbors returned by findBHclose */
  int max,min; /* min and max of these numbers */
  int nbr;     /* number of calls to findBHclose */
#endif
  short bfl;
} BHtree;

BHtree *generateBHtree(BHpoint **atoms, int nbat, int granularity);
BHnode *findBHnode(BHtree *tree,float *x);
int    findBHcloseAtoms(BHtree *tree,float *x,float cutoff,
		        int *atom,int maxn);
int    findBHcloseAtomsdist(BHtree *tree,float *x,float cutoff,
		            int *atom,float *d,int maxn);
void   freeBHtree(BHtree *tree);
void   divideBHnode(BHnode *node,float *xmin,float *xmax,int granularity);
void   freeBHnode(BHnode *node);

extern BHtree *bht;
extern BHpoint **BHat;

#endif




