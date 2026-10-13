/*
 * efit_aux.c
 */

#include <stdio.h>
#include <math.h>
#include "efit.h"

int
efit_read_points(eptr,points,fp)
struct efit_info *eptr;
struct datum *points;
FILE *fp;
{
    int i;
    int count;
    char buf[INBUFSIZE];
    float x,y,z,weight;

    /* init */
    if(!fp) return EFIT_ERROR;
    i = count = 0;
	if(debug){
		fprintf(stderr,"efit_read_points:\n");
	}

    /* read points */

    while(fgets(buf,sizeof(buf),fp) != NULL){
        if(sscanf(buf,"%f %f %f %f",&x,&y,&z,&weight) < 3){
            fprintf(stderr,"%s: error near line %d input [%s]\n",
				programname, i+1, buf);
            return EFIT_ERROR;
        }
        if(!eptr->weightflag) weight = 1.0;
        if(eptr->volumeflag) weight = weight*weight*weight;
        points[i].point[X] = x;
        points[i].point[Y] = y;
        points[i].point[Z]=  z;
        points[i].weight = weight;
        if(i >= MAXPOINTS){
            fprintf(stderr,"%s: too many points (max %d)\n",
                programname, MAXPOINTS);
            return EFIT_ERROR;
        }
        i++;
    }
    count = i;
    if(debug){
		fprintf(stderr,"\tcount %d\n", count);
	}
    if(count == 0){
        fprintf(stderr,"%s: null input\n",programname);
        return EFIT_ERROR;
    }
    if(count < 2){
        fprintf(stderr,"%s: too few points\n",programname);
        return EFIT_ERROR;
    }
    eptr->count = count;

    return EFIT_OK;
}

#ifdef EFIT_CANONICAL
efit_canonical()
{
    /* 
    orientation ambiguity: ??

    omega 0 == omega 180
    phi 0 == phi 180
    theta 0 = theta 180

    restrict range to +/- 90
    */
    if(ellipsoid.location.phi > 90.0){
        ellipsoid.location.phi = 180.0 - ellipsoid.location.phi;
    }
    if(ellipsoid.location.phi < -90.0){
        ellipsoid.location.phi = 180.0 + ellipsoid.location.phi;
    }
    if(ellipsoid.location.theta > 90.0){
        ellipsoid.location.theta = 180.0 - ellipsoid.location.theta;
    }
    if(ellipsoid.location.theta < -90.0){
        ellipsoid.location.theta = 180.0 + ellipsoid.location.theta;
    }
    if(ellipsoid.location.omega > 90.0){
        ellipsoid.location.omega = 180.0 - ellipsoid.location.omega;
    }
    if(ellipsoid.location.omega < -90.0){
        ellipsoid.location.omega = 180.0 + ellipsoid.location.omega;
    }

    location_matrix(&ellipsoid.location,m2);
    transtom33(m2,ellipsoid.orientation,&ellipsoid.position);
}
#endif

efit_init(eptr)
struct efit_info *eptr;
{
    if(!eptr) return EFIT_ERROR;
    eptr->cov_scale = 1.75;        /* multiplier for covariance std axies */
    eptr->ell_scale = 1.0;      /* multiplier for ellipsoid axies */
    return EFIT_OK;
}

efit_centroid(count,points,centroid)
int count;
struct datum *points;
float *centroid;
{
    /* return the centroid of a collection of points */
    int i;
    double sumx,sumy,sumz;

    if(!count) return EFIT_ERROR;
    sumx=sumy=sumz=0.0;
    for(i=0;i<count;i++){
        sumx=sumx+points[i].point[X];
        sumy=sumy+points[i].point[Y];
        sumz=sumz+points[i].point[Z];
    }
    centroid[X] = sumx/count;
    centroid[Y] = sumy/count;
    centroid[Z] = sumz/count;
    return EFIT_OK;
}

/* weight-scaled centroid */
efit_wcentroid(count,points,centroid)
int count;
struct datum *points;
float *centroid;
{
    int i;
    double sumx,sumy,sumz,weight,weightsum;

    if(!count) return EFIT_ERROR;
    sumx=sumy=sumz=0.0;
    weightsum = 0.0;
    for(i=0;i<count;i++){
        weight = points[i].weight;
        sumx=sumx+points[i].point[X]*weight;
        sumy=sumy+points[i].point[Y]*weight;
        sumz=sumz+points[i].point[Z]*weight;
        weightsum += weight;
    }
    centroid[X] = sumx/weightsum;
    centroid[Y] = sumy/weightsum;
    centroid[Z] = sumz/weightsum;
    return EFIT_OK;
}

/* reorder the orientation and the axies so that the principal
    components are ordered in x y z */

/* STRUCT */
struct pair {
    int index;
    float value;
};

pair_comparefn(a,b)
struct pair *a,*b;
{
    if(a->value > b->value) return(-1);
    if(a->value < b->value) return(1);
    return(0);
}

canonical_ellipsoid(eptr,ep)
struct efit_info *eptr;
struct ellipsoid *ep;
{
    struct pair a[3];
    int i,j;
    float dotvalue;
    matrix33 orientation;
    float row[3][3];
    float tmppoint[3];

    tmppoint[X] = tmppoint[Y] = tmppoint[Z]= 0.0;
    if(debug > 1)fprintf(stderr,"canonical_ellipsoid\n");

    /* prepare to sort axies lengths */
    mat_copy(ep->orientation,orientation);
    for(i=0;i<3;i++){
        a[i].index = i;
        a[i].value = ep->axis[i];
    }

    /* sort */
    if(!eptr->nosortflag)qsort((void *)a,3,sizeof(struct pair),pair_comparefn);

    /* set new axies */
    for(i=0;i<3;i++){
        ep->axis[i] = a[i].value;
        for(j=0;j<3;j++)ep->orientation[i][j] = orientation[a[i].index][j];
    }

    /* determine handedness of coordinate system */
    for(i=0;i<3;i++){
        row[i][X] = ep->orientation[i][0];
        row[i][Y] = ep->orientation[i][1];
        row[i][Z] = ep->orientation[i][2];
    }

    if(debug > 2){
        efit_print_point("row 0",row[0]);
        efit_print_point("row 1",row[1]);
        efit_print_point("row 2",row[2]);
    }

    vec_cross(row[1],row[2],tmppoint); /* y cross z */
    if(debug > 1)fprintf(stderr,"cross %g %g %g\n",
        tmppoint[X],tmppoint[Y],tmppoint[Z]);
    if((dotvalue = vec_dot(row[0],tmppoint)) < 0.0){
        /* invert handedness */
        if(debug > 1)fprintf(stderr,"inverting handedness\n");
        for(i=0;i<3;i++){
            for(j=0;j<3;j++) ep->orientation[i][j] *= -1.0;
        }
    }
    if(debug > 1)fprintf(stderr,"dotvalue %g\n",dotvalue);

    /* compute inverse mat */
    mat_transpose(ep->orientation,ep->inv_orientation);
	return EFIT_OK;
}

scale_ellipsoid(e,s)
struct ellipsoid *e;
float s;
{
    if(e == NULL){
        fprintf(stderr,"scale_ellipsoid: null ptr\n");
        return EFIT_ERROR;
    }
    e->axis[0] *= s;
    e->axis[1] *= s;
    e->axis[2] *= s;
	return EFIT_OK;
}

/*
 * given a points array and its centroid, compute inertia tensor
 * as a 3x3 matrix 
 */
void inertia_tensor(npoints,points,centroid,tensor,eptr)
int npoints;
struct datum *points;
float centroid[3];
matrix33 tensor;
struct efit_info *eptr;
{
    int i;
    double i11,i22,i33,i12,i31,i32;
    double ti11,ti22,ti33,ti12,ti31,ti32;
    float p[3];

    /* init */
    i11=i22=i33=i12=i31=i32=0.0;

    /* loop */
    for (i=0;i<npoints;i++){
        if(eptr->nocenterflag){
            p[X] = points[i].point[X];
            p[Y] = points[i].point[Y];
            p[Z] = points[i].point[Z];
        } else {
            vec_sub(points[i].point,centroid,p);
        }
        ti11 = p[Y]*p[Y] + p[Z]*p[Z];
        ti22 = p[X]*p[X] + p[Z]*p[Z];
        ti33 = p[Y]*p[Y] + p[X]*p[X];
        ti12 = p[X]*p[Y];
        ti31 = p[Z]*p[X];
        ti32 = p[Z]*p[Y];

        if(eptr->weightflag){
            i11 += points[i].weight * ti11;
            i22 += points[i].weight * ti22;
            i33 += points[i].weight * ti33;
            i12 += points[i].weight * ti12;
            i31 += points[i].weight * ti31;
            i32 += points[i].weight * ti32;
        } else {
            i11 += ti11;
            i22 += ti22;
            i33 += ti33;
            i12 += ti12;
            i31 += ti31;
            i32 += ti32;
        }
    }
    tensor[0][0] = i11;
    tensor[1][1] = i22;
    tensor[2][2] = i33;
    tensor[0][1] = tensor[1][0] = -i12;
    tensor[2][0] = tensor[0][2] = -i31;
    tensor[2][1] = tensor[1][2] = -i32;
}

efit_covar_1(npoints,points,centroid,tensor,eptr)
int npoints;
struct datum *points;
float centroid[3];
matrix33 tensor;
struct efit_info *eptr;
{
    int i;
    double i11,i22,i33,i12,i31,i32;
    double ti11,ti22,ti33,ti12,ti31,ti32;
    float p[3];

    /* init */
    i11=i22=i33=i12=i31=i32=0.0;
    if(debug)fprintf(stderr,"covar\n");

    /* loop */
    for (i=0;i<npoints;i++){
        vec_sub(points[i].point,centroid,p);
        ti11 = p[X]*p[X];
        ti22 = p[Y]*p[Y];
        ti33 = p[Z]*p[Z];
        ti12 = p[X]*p[Y];
        ti31 = p[Z]*p[X];
        ti32 = p[Z]*p[Y];

        if(eptr->weightflag){
            i11 += points[i].weight * ti11;
            i22 += points[i].weight * ti22;
            i33 += points[i].weight * ti33;
            i12 += points[i].weight * ti12;
            i31 += points[i].weight * ti31;
            i32 += points[i].weight * ti32;
        } else {
            i11 += ti11;
            i22 += ti22;
            i33 += ti33;
            i12 += ti12;
            i31 += ti31;
            i32 += ti32;
        }
    }
    tensor[0][0] = i11;
    tensor[1][1] = i22;
    tensor[2][2] = i33;
    tensor[0][1] = tensor[1][0] = i12;
    tensor[2][0] = tensor[0][2] = i31;
    tensor[2][1] = tensor[1][2] = i32;
}

efit_covar(npoints,points,centroid,covmat,eptr)
int npoints;
struct datum *points;
float centroid[3];
matrix33 covmat;
struct efit_info *eptr;
{
    int i;
    double i11,i22,i33,i12,i31,i32;
    double ti11,ti22,ti33,ti12,ti31,ti32;
    double sx,sy,sz;
    float p[3];

    /* init */
    sx=sy=sz=0.0;
    i11=i22=i33=i12=i31=i32=0.0;

    /* loop */
    if(debug)fprintf(stderr,"covar\n");
    for (i=0;i<npoints;i++){
        vec_sub(points[i].point,centroid,p);
        sx+=p[X];
        sy+=p[Y];
        sz+=p[Z];

        ti11 = p[X]*p[X];
        ti22 = p[Y]*p[Y];
        ti33 = p[Z]*p[Z];
        ti12 = p[X]*p[Y];
        ti31 = p[Z]*p[X];
        ti32 = p[Z]*p[Y];

        if(eptr->weightflag){
            i11 += points[i].weight * ti11;
            i22 += points[i].weight * ti22;
            i33 += points[i].weight * ti33;
            i12 += points[i].weight * ti12;
            i31 += points[i].weight * ti31;
            i32 += points[i].weight * ti32;
        } else {
            i11 += ti11;
            i22 += ti22;
            i33 += ti33;
            i12 += ti12;
            i31 += ti31;
            i32 += ti32;
        }
    }

    i11 = i11/(npoints - 1) - sx*sx/(npoints*npoints);
    i22 = i22/(npoints - 1) - sy*sy/(npoints*npoints);
    i33 = i33/(npoints - 1) - sz*sz/(npoints*npoints);

    i12 = i12/(npoints - 1) - sx*sy/(npoints*npoints);
    i31 = i31/(npoints - 1) - sz*sx/(npoints*npoints);
    i32 = i32/(npoints - 1) - sz*sy/(npoints*npoints);

    covmat[0][0] = i11;
    covmat[1][1] = i22;
    covmat[2][2] = i33;
    covmat[0][1] = covmat[1][0] = i12;
    covmat[2][0] = covmat[0][2] = i31;
    covmat[2][1] = covmat[1][2] = i32;

	return EFIT_OK;
}

/*
 * fit an ellipsoid to a collection of points by using moments of inertia
 */

fit_ellipsoid(npoints,points, ellipsoid,eptr)
int npoints;
struct datum *points;
struct ellipsoid *ellipsoid;
struct efit_info *eptr;
{
    int i;
    double sum, weightsum;
    float eigenvalues[3];

	/* init */
	if(debug)fprintf(stderr,"fit_ellipsoid:\n");

    weightsum = 0.0;
    for(i=0;i<npoints;i++)weightsum += points[i].weight;
    if(eptr->weightflag){
        efit_wcentroid(npoints,points,ellipsoid->position);
    } else {
        efit_centroid(npoints,points,ellipsoid->position);
    }
    if(eptr->covarflag) {
        efit_covar(npoints,points,ellipsoid->position,ellipsoid->tensor,eptr);
    } else {
        inertia_tensor(npoints,points,ellipsoid->position,ellipsoid->tensor,eptr);
    }
    if(debug > 1){
        fprintf(stderr,"centroid: %f %f %f\n",
            ellipsoid->position[X],
			ellipsoid->position[Y],
			ellipsoid->position[Z]);
        fprintf(stderr,"tensor\n");
        for(i=0;i<3;i++)fprintf(stderr,"%f %f %f\n",
            ellipsoid->tensor[i][0],
            ellipsoid->tensor[i][1],
            ellipsoid->tensor[i][2]);
    }

    (void)mat_jacobi(ellipsoid->tensor,eigenvalues,ellipsoid->inv_orientation);

    /* NB jacobi returns inverse matrix -> transpose to get normal mat */
    mat_transpose(ellipsoid->inv_orientation,ellipsoid->orientation);
    sum = eigenvalues[0] + eigenvalues[1] + eigenvalues[2];

    /* from the principal moments, compute equivalent physical ellipsoid */
    if(debug){
        fprintf(stderr,"eigenvalues: %g %g %g\n",
            eigenvalues[0],eigenvalues[1],eigenvalues[2]);
        if(debug > 1)fprintf(stderr,"weightsum %g\n", weightsum);
    }
    if(eptr->covarflag){
        if(debug){
            fprintf(stderr,"std_deviation: %g %g %g\n",
                sqrt((double)eigenvalues[0]),
                sqrt((double)eigenvalues[1]),
                sqrt((double)eigenvalues[2]));
        }
        /* scale sigma axies */
        for(i=0; i<3; i++){
            ellipsoid->axis[i] = eptr->cov_scale*sqrt((double)eigenvalues[i]);
        }
    } else {
        for(i=0; i<3; i++){
            ellipsoid->axis[i] = 
                sqrt((double)(5./(2.*weightsum))*(sum - 2.*eigenvalues[i]));
        }
    }
	return EFIT_OK;
}

/* compute the radius of gyration of an ellipsoid */
float 
gyration_of_ellipsoid(ellipsoid)
struct ellipsoid ellipsoid;
{
    double sum;
    sum = (ellipsoid.axis[0] * ellipsoid.axis[0]) + 
        (ellipsoid.axis[1] * ellipsoid.axis[1]) +
        (ellipsoid.axis[2] * ellipsoid.axis[2]);
    return((double)sqrt(sum/5.));
}

/*
m33totrans(m1,position,m2)
matrix33 m1;
float position*;
transform_matrix m2;
{
    m2[0][0] = m1[0][0];
    m2[0][1] = m1[0][1];
    m2[0][2] = m1[0][2];

    m2[1][0] = m1[1][0];
    m2[1][1] = m1[1][1];
    m2[1][2] = m1[1][2];

    m2[2][0] = m1[2][0];
    m2[2][1] = m1[2][1];
    m2[2][2] = m1[2][2];

    m2[3][0] = position[X];
    m2[3][1] = position[Y];
    m2[3][2] = position[Z];
}
*/

/*
transtom33(m1,m2,position)
transform_matrix m1;
matrix33 m2;
struct point position;
{
    m2[0][0] = m1[0][0];
    m2[0][1] = m1[0][1];
    m2[0][2] = m1[0][2];

    m2[1][0] = m1[1][0];
    m2[1][1] = m1[1][1];
    m2[1][2] = m1[1][2];

    m2[2][0] = m1[2][0];
    m2[2][1] = m1[2][1];
    m2[2][2] = m1[2][2];

    position[X] = m1[3][0];
    position[Y] = m1[3][1];
    position[Z] = m1[3][2];
}
*/

/* end of file */
