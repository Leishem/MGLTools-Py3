%module xtcparser
%feature("python:cdefaultargs");

%{
// this file contains methods for reading GROMACS xtc trajectory files.

#if (defined WIN32 || defined _WIN32 || defined WIN64  || defined _WIN64 || defined __CYGWIN__ || defined __CYGWIN32__ || defined GMX_INTERNAL_XDR)
#include "gmx_system_xdr.h"
#else
#include <rpc/types.h>
#include <rpc/xdr.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "xtcio.h"

#define INDENT		3
#define true (1==1)
#define false (!true)

typedef struct
{
  FILE * fp;
  XDR xdrs;
} fileid;

 static fileid FIO;

int pr_indent(FILE *fp,int n)
{
  int i;

  for (i=0; i<n; i++) (void) fprintf(fp," ");
  return n;
}

int pr_title_nxn(FILE *fp,int indent,const char *title,int n1,int n2)
{
  (void) pr_indent(fp,indent);
  (void) fprintf(fp,"%s (%dx%d):\n",title,n1,n2);
  return (indent+INDENT);
}

int pr_title(FILE *fp,int indent,const char *title)
{
  (void) pr_indent(fp,indent);
  (void) fprintf(fp,"%s:\n",title);
  return (indent+INDENT);
}

int available(FILE *fp,void *p,const char *title)
{
  if (!p) (void) fprintf(fp,"not available: %s\n",title);
  return (p!=NULL);
}

void pr_rvecs(FILE *fp,int indent,const char *title,rvec vec[],int n)
{

  char *format = "%12.5e";
  int i,j;

    
  if (available(fp,vec,title)) {  
    indent=pr_title_nxn(fp,indent,title,n,DIM);
    for (i=0; i<n; i++) {
      (void) pr_indent(fp,indent);
      (void) fprintf(fp,"%s[%5d]={",title,i);
      for (j=0; j<DIM; j++) {
	if (j != 0) 
	  (void) fprintf(fp,", ");
	(void) fprintf(fp,format,vec[i][j]);
      }
      (void) fprintf(fp,"}\n");
    }
  }
}

void read_xtc_out(char *fn, bool bXVG)
{
  int    indent;
  char   buf[256];
  rvec   *x;
  matrix box;
  int    nframe,natoms,step;
  real   prec,time;
  bool   bOK;
  FILE           *fp;
  XDR xdrs;
  fp = fopen(fn, "r");
  xdrstdio_create(&xdrs, fp, XDR_DECODE);
  read_first_xtc(&xdrs,&natoms,&step,&time,box,&x,&prec,&bOK);
  return;
  nframe=0;
  do {
    if (bXVG) {
      int i,d;
      
      fprintf(stdout,"%g",time);
      for(i=0; i<natoms; i++)
	for(d=0; d<DIM; d++)
	  fprintf(stdout," %g",x[i][d]);
      fprintf(stdout,"\n");
    } else {
      sprintf(buf,"%s frame %d",fn,nframe);
      indent=0;
      indent=pr_title(stdout,indent,buf);
      pr_indent(stdout,indent);
      fprintf(stdout,"natoms=%10d  step=%10d  time=%10g  prec=%10g\n",
	    natoms,step,time,prec);
      pr_rvecs(stdout,indent,"box",box,DIM);
      pr_rvecs(stdout,indent,"x",x,natoms);
    }
    nframe++;
  } while (read_next_xtc(&xdrs,natoms,&step,&time,box,x,&prec,&bOK));
  if (!bOK)
    fprintf(stderr,"\nWARNING: Incomplete frame at time %g\n",time);
  xdr_destroy(&xdrs);
  fclose(fp);

}





void open_xtc(char *fn)

{

  FIO.fp = fopen(fn, "r");
  xdrstdio_create(&(FIO.xdrs), FIO.fp, XDR_DECODE);

}

int read_header_xtc(int *magic,int *natoms,int *step,real *time)
{
  bool   bOK;
  return xtc_header(&(FIO.xdrs),magic,natoms,step,time,&bOK);
}


int read_first_frame_xtc(int magic, float coords[][3], int natoms,
			 real *prec)
{

  rvec   *x;
  matrix box;
  int i,j,indent;
  char   buf[256];
  int bOK=TRUE;
  int res;
  /* Check magic number */
  if (! check_xtc_magic(magic))
    return 0;
  
  snew(x,natoms);

  bOK =xtc_coord(&(FIO.xdrs), &natoms,box,x,prec,TRUE);
  for (i=0; i<natoms; i++) {
      for (j=0; j<DIM; j++) {
	coords[i][j] = x[i][j];
      }
    }
  sfree(x);
  return bOK;
  
}

int read_next_frame_xtc(float coords[][3], int natoms,
			int *step,real *time, real *prec)
{
  int magic;
  int n, i, j;
  matrix box;
  rvec   *x;
  int bOK =TRUE;
  /* read header */
  if (!xtc_header(&(FIO.xdrs),&magic,&n,step,time,&bOK))
    return 0;
  if (n>natoms)
    {
      fprintf(stderr, "Frame contains more atoms (%d) than expected (%d)", 
	      n, natoms);
      return 0;
    }
      
  /* Check magic number */
  check_xtc_magic(magic);
  snew(x,natoms);
  bOK=xtc_coord(&(FIO.xdrs),&natoms,box,x,prec,TRUE);

  for (i=0; i<natoms; i++) {
      for (j=0; j<DIM; j++) {
	coords[i][j] = x[i][j];
      }
    }
  
  sfree(x);
  return bOK;
}
void close_xtc()
{
  if (&(FIO.xdrs))
    {
      xdr_destroy(&(FIO.xdrs));
    }
  if (FIO.fp)
    {
      fclose(FIO.fp);
      FIO.fp = 0;
    }
}
	       
%}

%include numarr.i
%include typemaps.i


void read_xtc_out(char *fn, bool bXVG);
void open_xtc(char *fn);

//void open_xtc(char *fn, fileid *fi);

int read_header_xtc(int *OUTPUT,int *OUTPUT, int *OUTPUT, real *OUTPUT);


FLOAT_ARRAY2D(coords, [1][3], natoms)

#ifdef GMX_DOUBLE
     typedef double real;
#else
      typedef float  real;
#endif


int read_first_frame_xtc(int magic, float coords[1][3], int natoms, real *OUTPUT);
int read_next_frame_xtc(float coords[1][3], int natoms,  
                        int *OUTPUT, real *OUTPUT, real *OUTPUT);

void close_xtc();

%pythoncode %{
#
# this Python function uses the module interface for reading an xtc trajectory file
# the function does not check if file exists or if it has .xtc extension
#
def read_xtc(file):

    nframes = 0
    _xtcparser.open_xtc(file)
    headers = []
    status, magic, natoms, step, time =_xtcparser.read_header_xtc()
    if not status:
        print("Error in reading header of file %s"%file)
        return 0
    import numpy
    cs = numpy.zeros((natoms, 3), "f")
    status, prec = _xtcparser.read_first_frame_xtc(magic, cs)
    coords=[]

    if not status:
        print("Error in reading frame %d"%nframes)
        return 0
    while status:
        nframes = nframes + 1
        coords.append(cs.astype("f")) # to make a copy of the array
        headers.append({'frame':nframes, 'step':step, 'time':time, 'prec':prec})
        status, step,time,prec = _xtcparser.read_next_frame_xtc(cs)

    return coords, headers
%}