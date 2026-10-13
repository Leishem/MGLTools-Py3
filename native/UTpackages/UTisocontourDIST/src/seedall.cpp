//------------------------------------------------------------------------
//
// seedAll.C - preprocessing of 2d volumes for seed set extraction
//
//------------------------------------------------------------------------

// $Id: seedall.cpp,v 1.3 2008/09/19 22:04:17 annao Exp $

#include <stdlib.h>
#if ! defined (__APPLE__)
#include <malloc.h>
#endif
#include <memory.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "seedall.h"
#include "datareg2.h"

#define DEBUGNo

extern int verbose;

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void
seedAll::compSeeds(void)
{
   u_int c;
   float min, max;
   int nseed;

   if (verbose)
      printf("***** Seed Creation\n");

   // proceed through the slices computing seeds
   nseed=0;

   for (c=0; c<data.getNCells(); c++) {

         // load the voxel data
         data.getCellRange(c, min, max);

         seeds.AddSeed(c, min, max);

         nseed++;
   }

   if (verbose)
      printf("computed %d seeds\n", nseed);
}
