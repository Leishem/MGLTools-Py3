/*
 * 
 *                This source code is part of
 * 
 *                 G   R   O   M   A   C   S
 * 
 *          GROningen MAchine for Chemical Simulations
 * 
 *                        VERSION 3.2.0
 * Written by David van der Spoel, Erik Lindahl, Berk Hess, and others.
 * Copyright (c) 1991-2000, University of Groningen, The Netherlands.
 * Copyright (c) 2001-2004, The GROMACS development team,
 * check out http://www.gromacs.org for more information.

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * If you want to redistribute modifications, please consider that
 * scientific software is very special. Version control is crucial -
 * bugs must be traceable. We will be happy to consider code for
 * inclusion in the official distribution, but derived work must not
 * be called official GROMACS. Details are found in the README & COPYING
 * files - if they are missing, get the official version at www.gromacs.org.
 * 
 * To help us fund GROMACS development, we humbly ask that you cite
 * the papers on the package - you can find them in the top README file.
 * 
 * For more info, check our website at http://www.gromacs.org
 * 
 * And Hey:
 * GROningen Mixture of Alchemy and Childrens' Stories
 */


/* This file is modified by Anna Omelchenko, MGL, TSRI */

#include "xtcio.h"

void *save_calloc(char *name,char *file,int line,
                  unsigned nelem,unsigned elsize)
{
  void *p;
  
  p=NULL;
  if ((nelem==0)||(elsize==0))
    p=NULL;
  else
    {
      if ((p=calloc((size_t)nelem,(size_t)elsize))==NULL) 
        fprintf(stderr, "calloc for %s (nelem=%d, elsize=%d, file %s" ", line %d)",name,nelem,elsize,file,line);
    }

  return p;
}

void save_free(char *name,char *file,int line, void *ptr)
{
  if (ptr != NULL)
    free(ptr);
}


int check_xtc_magic(int magic)
{
  if (magic != XTC_MAGIC)
    { 
      fprintf(stderr,"Magic Number Error in XTC file (read %d, should be %d)",
	      magic,XTC_MAGIC);
      return 0;
    }
  else 
    return 1;
  
}

int xtc_check(char *str,bool bResult,char *file,int line)
{
  if (!bResult) 
    {
      fprintf(stderr,"\nXTC error: read/write of %s failed, "
	      "source file %s, line %d\n",str,file,line);
      return 0;
    }
  return 1;
}

#define XTC_CHECK(s,b) xtc_check(s,b,__FILE__,__LINE__)

int xtc_header(XDR *xd,int *magic,int *natoms,int *step,real *time,
		      bool *bOK)
{
  int result;

  if (xdr_int(xd,magic) == 0)
      return 0;
  result=XTC_CHECK("natoms", xdr_int(xd,natoms));  /* number of atoms */
  if (result)
    result=XTC_CHECK("step",   xdr_int(xd,step));    /* frame number    */
  if (result)
    result=XTC_CHECK("time",   xdr_real(xd,time));   /* time            */
  *bOK=(result!=0);

  return result;
}

int xtc_coord(XDR *xd,int *natoms,matrix box,rvec *x,real *prec, bool bRead)
{
  int i,j,result;
#ifdef GMX_DOUBLE
  float *ftmp;
  float fprec;
#endif
    
  /* box */
  result=1;
  for(i=0; ((i<DIM) && result); i++)
    for(j=0; ((j<DIM) && result); j++)
      result=XTC_CHECK("box",xdr_real(xd,&(box[i][j])));
  
  if (!result)
      return result;
#ifdef GMX_DOUBLE
  /* allocate temp. single-precision array */
  snew(ftmp,(*natoms)*DIM);
  
  /* Copy data to temp. array if writing */
  if(!bRead)
  {
      for(i=0; (i<*natoms); i++)
      {
          ftmp[DIM*i+XX]=x[i][XX];      
          ftmp[DIM*i+YY]=x[i][YY];      
          ftmp[DIM*i+ZZ]=x[i][ZZ];      
      }
      fprec = *prec;
  }
  result=XTC_CHECK("x",xdr3dfcoord(xd,ftmp,natoms,&fprec));
  
  /* Copy from temp. array if reading */
  if(bRead)
  {
      for(i=0; (i<*natoms); i++)
      {
          x[i][XX] = ftmp[DIM*i+XX];      
          x[i][YY] = ftmp[DIM*i+YY];      
          x[i][ZZ] = ftmp[DIM*i+ZZ];      
      }
      *prec = fprec;
  }  
  sfree(ftmp);
#else
    result=XTC_CHECK("x",xdr3dfcoord(xd,x[0],natoms,prec)); 
#endif 
  return result;
}



int read_first_xtc(XDR *xd,int *natoms,int *step,real *time,
		   matrix box,rvec **x,real *prec,bool *bOK)
{
  int magic;
  
  *bOK=TRUE;
  
  /* read header and malloc x */
  if ( !xtc_header(xd,&magic,natoms,step,time,bOK))
    return 0;
  return 1;
    
  /* Check magic number */
  if (! check_xtc_magic(magic))
    return 0;
  
  snew(*x,*natoms);

  *bOK=xtc_coord(xd,natoms,box,*x,prec,TRUE);
  
  return *bOK;
}

int read_next_xtc(XDR *xd,
		  int natoms,int *step,real *time,
		  matrix box,rvec *x,real *prec,bool *bOK)
{
  int magic;
  int n;

  *bOK=TRUE;
  
  /* read header */
  if (!xtc_header(xd,&magic,&n,step,time,bOK))
    return 0;
  if (n>natoms)
    {
      fprintf(stderr, "Frame contains more atoms (%d) than expected (%d)", 
	      n, natoms);
      return 0;
    }
      
  /* Check magic number */
  check_xtc_magic(magic);

  *bOK=xtc_coord(xd,&natoms,box,x,prec,TRUE);

  return *bOK;
}

