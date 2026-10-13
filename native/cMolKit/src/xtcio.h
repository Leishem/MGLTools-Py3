/*
 * $Id: xtcio.h,v 1.1.1.1 2006/10/10 22:13:22 annao Exp $
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
 * Gromacs Runs On Most of All Computer Systems
 */

/* This file is modified by Anna Omelchenko, MGL, TSRI */

#ifndef _xtcio_h
#define _xtcio_h 


#include <stdio.h>
#include <stdlib.h>

#ifdef CPLUSPLUS
extern "C" {
#endif

#ifndef XTC_MAGIC
#define XTC_MAGIC 1995
#endif

#ifdef CPLUSPLUS
extern "C" {
#endif

#include "xdrf.h"

void *save_calloc(char *name,char *file,int line, unsigned nelem,unsigned elsize);
void save_free(char *name,char *file,int line, void *ptr);

#define snew(ptr,nelem) (ptr)=save_calloc(#ptr,__FILE__,__LINE__,\
			(nelem),sizeof(*(ptr)))
#define sfree(ptr) save_free(#ptr,__FILE__,__LINE__,(ptr))


int xtc_header(XDR *xd,int *magic,int *natoms,int *step,real *time,
	       bool *bOK);
extern int check_xtc_magic(int magic);

extern int xtc_coord(XDR *xd,int *natoms,matrix box,rvec *x,real *prec, bool bRead);
extern int xtc_check(char *str,bool bResult,char *file,int line);

/* read xtc file first time, allocate memory for x */

extern int read_first_xtc(XDR *xd,int *natoms,int *step,real *time,
			  matrix box,rvec **x,real *prec,bool *bOK);

/* Read subsequent frames */
extern int read_next_xtc(XDR *xd,
			 int natoms,int *step,real *time,
			 matrix box,rvec *x,real *prec,bool *bOK);

#ifdef CPLUSPLUS
}
#endif

#endif
