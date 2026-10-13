/*
 * efit_io.c
 */

#include <stdio.h>
#include "efit.h"

efit_print_point(s,p)
char *s;
float *p;
{
    printf("%s: %g %g %g\n",s,p[X],p[Y],p[Z]);
}

/* print an ellipsoid matrix on fp */
/* includes scalefactors */
print_ellipsoid_matrix(fp, ellipsoid,eptr)
FILE *fp;
struct ellipsoid ellipsoid;
struct efit_info *eptr;
{
    if(!eptr->noscaleflag)
        fprintf(fp,
            MATRIXFORMAT,
            ellipsoid.orientation[0][0]*ellipsoid.axis[0],
            ellipsoid.orientation[0][1]*ellipsoid.axis[0],
            ellipsoid.orientation[0][2]*ellipsoid.axis[0],
            ellipsoid.orientation[1][0]*ellipsoid.axis[1],
            ellipsoid.orientation[1][1]*ellipsoid.axis[1],
            ellipsoid.orientation[1][2]*ellipsoid.axis[1],
            ellipsoid.orientation[2][0]*ellipsoid.axis[2],
            ellipsoid.orientation[2][1]*ellipsoid.axis[2],
            ellipsoid.orientation[2][2]*ellipsoid.axis[2]);
    else
        fprintf(fp,
            MATRIXFORMAT,
            ellipsoid.orientation[0][0],
            ellipsoid.orientation[0][1],
            ellipsoid.orientation[0][2],
            ellipsoid.orientation[1][0],
            ellipsoid.orientation[1][1],
            ellipsoid.orientation[1][2],
            ellipsoid.orientation[2][0],
            ellipsoid.orientation[2][1],
            ellipsoid.orientation[2][2]);
    fprintf(fp, POSITIONFORMAT,
        ellipsoid.position[X], ellipsoid.position[Y], ellipsoid.position[Z]);
}

print_ellipsoid_tensor(fp, ellipsoid,eptr)
FILE *fp;
struct ellipsoid ellipsoid;
struct efit_info *eptr;
{
    fprintf(fp,
        MATRIXFORMAT,
        ellipsoid.tensor[0][0],
        ellipsoid.tensor[0][1],
        ellipsoid.tensor[0][2],
        ellipsoid.tensor[1][0],
        ellipsoid.tensor[1][1],
        ellipsoid.tensor[1][2],
        ellipsoid.tensor[2][0],
        ellipsoid.tensor[2][1],
        ellipsoid.tensor[2][2]);
    fprintf(fp, POSITIONFORMAT,
        ellipsoid.position[X], ellipsoid.position[Y], ellipsoid.position[Z]);
}

print_ellipsoid_location(fp, ellipsoid,eptr)
FILE *fp;
struct ellipsoid ellipsoid;
struct efit_info eptr;
{
    fprintf(fp,
        MATRIXFORMAT,
        ellipsoid.orientation[0][0],
        ellipsoid.orientation[0][1],
        ellipsoid.orientation[0][2],
        ellipsoid.orientation[1][0],
        ellipsoid.orientation[1][1],
        ellipsoid.orientation[1][2],
        ellipsoid.orientation[2][0],
        ellipsoid.orientation[2][1],
        ellipsoid.orientation[2][2]);
    fprintf(fp, POSITIONFORMAT,
        ellipsoid.position[X], ellipsoid.position[Y], ellipsoid.position[Z]);
}

print_ellipsoid(fp, ellipsoid,eptr)
FILE *fp;
struct ellipsoid ellipsoid;
struct efit_info *eptr;
{
    fprintf(fp, "ellipsoid\nposition %.4f %.4f %.4f\naxis %.4f %.4f %.4f\n",
        ellipsoid.position[X],
        ellipsoid.position[Y],
        ellipsoid.position[Z],
        ellipsoid.axis[X],
        ellipsoid.axis[Y],
        ellipsoid.axis[Z]);
    fprintf(fp,
        "orientation\n%.5f %.5f %.5f\n%.5f %.5f %.5f\n%.5f %.5f %.5f\n",
        ellipsoid.orientation[0][0],
        ellipsoid.orientation[0][1],
        ellipsoid.orientation[0][2],
        ellipsoid.orientation[1][0],
        ellipsoid.orientation[1][1],
        ellipsoid.orientation[1][2],
        ellipsoid.orientation[2][0],
        ellipsoid.orientation[2][1],
        ellipsoid.orientation[2][2]);
}

/* end of file */
