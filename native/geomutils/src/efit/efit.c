/*
 * efit pointfile
 *
 * fit an inertial ellipsoid to input data points
 */

#include <stdio.h>
#include <math.h>

#define EFIT_MAIN 1

#include "efit.h"

/* globals */
static char helpstring[] = 
"-NoScale -Cscale cscale -covar\n\t-volume -matrix -scale escale\n\t-weightflag -nocenter pointfile";

/* MAIN */

#include <stdlib.h>
int fitEllipse(float *pts, int nbpts, float ell_scale, float cov_scale,
	       struct efit_info *eptr, struct ellipsoid *ellipsoid)
{
  int i;
  float weight, *pt=pts;

  if(efit_init(eptr) != EFIT_OK) return EFIT_ERROR;
  if (cov_scale > 0.) {
    eptr->cov_scale = cov_scale;      /* multiplier for covariance std axies */
    eptr->covarflag++;
  }
  if (ell_scale > 0.)
    eptr->ell_scale = ell_scale;      /* multiplier for ellipsoid axies */
  
  if(nbpts >= MAXPOINTS){
    fprintf(stderr,"%s: too many points (max %d)\n",
	    programname, MAXPOINTS);
    return EFIT_ERROR;
  }
  if(nbpts == 0){
    fprintf(stderr,"%s: null input\n",programname);
    return EFIT_ERROR;
  }
  if(nbpts < 2){
    fprintf(stderr,"%s: too few points\n",programname);
    return EFIT_ERROR;
  }

  if(!eptr->weightflag) weight = 1.0;
  if(eptr->volumeflag) weight = weight*weight*weight;
  for (i=0; i<nbpts; i++) {
    ellipsedata[i].point[X] = *pt; pt++;
    ellipsedata[i].point[Y] = *pt; pt++;
    ellipsedata[i].point[Z] = *pt; pt++;
    ellipsedata[i].weight = weight;
    /*
    fprintf(stdout, "%d %9.5f %9.5f %9.5f %g\n", i,
	    ellipsedata[i].point[X],
	    ellipsedata[i].point[Y],
	    ellipsedata[i].point[Z],
	    ellipsedata[i].weight);
    */
  }
  eptr->count = nbpts;
  
  if(fit_ellipsoid(eptr->count, ellipsedata, ellipsoid, eptr) != EFIT_OK){
    return(EFIT_ERROR);
  }
  scale_ellipsoid(ellipsoid,eptr->ell_scale);

  /*print_ellipsoid(stdout,ellipsoid,eptr);*/
  return(EFIT_OK);
}

main(argc,argv)
int argc;
char **argv;
{
    int argindex;
    FILE *fp;
    char *filename = NULL;
    struct ellipsoid ellipsoid;
    struct efit_info *eptr;

    /* init */
    eptr = &efit_info;

	if(efit_init(eptr) != EFIT_OK) return EFIT_ERROR;

    /* get args */
    argindex = efit_setflags(argc,argv,eptr);

    if((argc == 1) && isatty(0)) efit_usage();

	/* get filename if any*/
    if(argindex == (argc - 1)){
        filename = argv[argindex];
        if((fp = fopen(filename,"r")) == NULL){
            fprintf(stderr,"fit: can't open %s\n", filename);
            return EFIT_ERROR;
        }
        strcpy(ellipsoid.name,argv[argindex]);
    } else fp = stdin;

    if(debug){
        fprintf(stderr,"%s:\n",programname);
        if(debug > 1)fprintf(stderr,"\tdebug %d\n",debug);
        if(filename)fprintf(stderr,"\tfilename %s\n",filename);
        if(testflag)fprintf(stderr,"\ttestflag %d\n",testflag);
        if(eptr->weightflag)fprintf(stderr,"\tweightflag %d\n",eptr->weightflag);
        if(eptr->volumeflag)fprintf(stderr,"\tvolumeflag %d\n",eptr->volumeflag);
        if(eptr->nocenterflag)fprintf(stderr,"\tnocenterflag %d\n",
            eptr->nocenterflag);
        if(eptr->noscaleflag)fprintf(stderr,"\tnoscaleflag %d\n",
            eptr->noscaleflag);
        if(eptr->nosortflag)fprintf(stderr,"\tnosortflag %d\n",eptr->nosortflag);
        fprintf(stderr,"\tell_scale %g\n",eptr->ell_scale);
        fprintf(stderr,"\tcov_scale %g\n",eptr->cov_scale);
    }

    if(efit_read_points(eptr,ellipsedata,fp) != EFIT_OK) return(EFIT_ERROR);

    if(fit_ellipsoid(eptr->count,ellipsedata,&ellipsoid,eptr) != EFIT_OK){
        return(EFIT_ERROR);
    }

    /* define canonical orientation largest axies x > y > z */
    if(eptr->nosortflag)canonical_ellipsoid(eptr,&ellipsoid);

    if(!eptr->noscaleflag)scale_ellipsoid(&ellipsoid,eptr->ell_scale);

    /* print results */
	if(debug) print_ellipsoid(stderr,ellipsoid,eptr);
    if(eptr->matrixflag){
        /* print transformation matrix for ellipsoid */
        print_ellipsoid_matrix(stdout,ellipsoid,eptr);
    } else if(eptr->covarflag > 1){
        print_ellipsoid_tensor(stdout,ellipsoid,eptr);
    } else {
        print_ellipsoid(stdout,ellipsoid,eptr);
    }
    return(EFIT_OK);
}

/*
 * efit_setflags
 *
 * read flags from argv 
 * return index of first non arg
 */

efit_setflags(argc,argv,eptr)
int argc;
char **argv;
struct efit_info *eptr;
{
    int argindex;
    /* init */
    argindex = 1;
    programname = argv[0];
    if(!eptr) {
        fprintf(stderr,"efit_setflags: null eptr\n");
        exit(1);
    }
    /* loop over args */
    while((argc > 1) && (argv[1][0] == '-')){
        switch(argv[1][1]){
        case 'C':
            eptr->cov_scale = atof(argv[2]);
            argv++;
            argc--;
            argindex++;
            break;
        case 's':
            eptr->ell_scale = atof(argv[2]);
            argv++;
            argc--;
            argindex++;
            break;
        case 'c':
            eptr->covarflag++;
            break;
        case 'v':
            eptr->volumeflag++;
            break;
        case 'w':
            eptr->weightflag++;
            break;
        case 'N':
            eptr->noscaleflag++;
            break;
        case 'S':
            eptr->nosortflag++;
            break;
        case 'm':
            eptr->matrixflag++;
            break;
        case 'n':
            eptr->nocenterflag++;
            break;
        case 't':
            testflag++;
            break;
        case 'd':
            debug++;
            break;
        case 'u':
            efit_usage();
            break;
        default:
            fprintf(stderr, "%s: unknown switch -%c\n",programname,argv[1][1]);
            exit(1);
            break;
        }
        argindex++;
        argc--;
        argv++;
    }
    return(argindex);
}

efit_usage()
{
    fprintf(stderr,"usage: %s %s\n",programname,helpstring);
    exit(0);
}

/* end of file */
