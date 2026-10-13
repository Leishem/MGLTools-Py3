/*
* vec.c    vector/matrix functions		float args and types
*/

#include <stdio.h>
#include <math.h>
#include "vec.h"

#define VEC_DEG_TO_RAD(x) ((x)*M_PI/180.0)
#define VEC_RAD_TO_DEG(x) ((x)*180.0/M_PI)

void
mat_identity(float (*m)[3])
{
    m[0][0] = m[1][1] = m[2][2] = 1.0;
    m[0][1] = m[0][2] = 0.0;
    m[1][0] = m[1][2] = 0.0;
    m[2][0] = m[2][1] = 0.0;
}

void 
mat_transpose(float (*m1)[3], float (*m2)[3])
{
    m2[0][0] = m1[0][0];
    m2[0][1] = m1[1][0];
    m2[0][2] = m1[2][0];

    m2[1][0] = m1[0][1];
    m2[1][1] = m1[1][1];
    m2[1][2] = m1[2][1];

    m2[2][0] = m1[0][2];
    m2[2][1] = m1[1][2];
    m2[2][2] = m1[2][2];
}

float
vec_length(float *p)
{
    return(VEC_SQRT(vec_dot(p,p)));
}

void 
mat_copy(float (*m1)[3], float (*m2)[3])
               
/* m2 = m1 */
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
}

void 
mat_mult(float (*m1)[3], float (*m2)[3], float (*m)[3])
                 
/*    return m= m1 * m2    */
/*    NOTE:  THE DESTINATION MATRIX MUST NOT BE ONE OF THE SOURCES */
{

    m[0][0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0];
    m[1][0] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0];
    m[2][0] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0];

    m[0][1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1];
    m[1][1] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1];
    m[2][1] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1];

    m[0][2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2];
    m[1][2] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2];
    m[2][2] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2];
} 

void 
vec_transform(float *p, float (*m)[3], float *q)
           
            
/*    return  p *  m [premultiplication] */
{
    float x,y,z;
    x = p[0];
    y = p[1];
    z = p[2];
    q[0] = m[0][0]*x + m[1][0]*y + m[2][0]*z;
    q[1] = m[0][1]*x + m[1][1]*y + m[2][1]*z;
    q[2] = m[0][2]*x + m[1][2]*y + m[2][2]*z;
}

void 
vec_ftransform(float *p, float (*m)[3], float *q)
/*    return  p *  m [premultiplication] */
/* src != dest */
{
    q[0] = m[0][0]*p[0] + m[1][0]*p[1] + m[2][0]*p[2];
    q[1] = m[0][1]*p[0] + m[1][1]*p[1] + m[2][1]*p[2];
    q[2] = m[0][2]*p[0] + m[1][2]*p[1] + m[2][2]*p[2];
}

void 
vec_zero(float *p)
{
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = 0.0;
}

void 
vec_scale(float f, float *p1, float *p2)
{
    p2[0] = f*p1[0];
    p2[1] = f*p1[1];
    p2[2] = f*p1[2];
}

void 
vec_sub(float *p1, float *p2, float *p)
{
    p[0] = p1[0] - p2[0];
    p[1] = p1[1] - p2[1];
    p[2] = p1[2] - p2[2];
}

void 
vec_add(float *p1, float *p2, float *p)
{
    p[0] = p1[0] + p2[0];
    p[1] = p1[1] + p2[1];
    p[2] = p1[2] + p2[2];
}

void 
vec_copy(float *p1, float *p2)
{
    p2[0] = p1[0];
    p2[1] = p1[1];
    p2[2] = p1[2];
}

float
vec_dot(float *p1, float *p2)
{
    return(p1[0]*p2[0] + p1[1]*p2[1] + p1[2]*p2[2]);
}

int
vec_normalize(float *p)
{
    float sum;
    sum = p[0]*p[0] + p[1]*p[1] + p[2]*p[2];
    if(sum < VEC_FUZZ)return -1;
    sum = VEC_SQRT(sum);
    p[0] /= sum;
    p[1] /= sum;
    p[2] /= sum;
	return 0;
}

float
vec_magsq(float *p)
{
    return(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
}

float
vec_mag(float *p)
{
    return(VEC_SQRT(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]));
}

void
vec_transform_points(int count, float *src, float *dest, float (*matrix)[3])
{
    int i;
    for(i=0;i<count;i++){
        vec_transform(src++,matrix,dest++);
	}
}

void
vec_cross(float *a, float *b, float *c)
                 
/* c = a x b */
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

float 
vec_distance(float *p1, float *p2)
{
    float delta;
    float sum;
    sum = p1[0] - p2[0];
    sum *= sum;
    delta = p1[1] - p2[1];
    delta *= delta;
    sum += delta;
    delta = p1[2] - p2[2];
    delta *= delta;
    sum += delta;
    return(VEC_SQRT(sum));
}

float 
vec_distancesq(float *p1, float *p2)
{
    float sum;
    float delta;
    delta = p1[0] - p2[0];
    sum = delta*delta;
    delta = p1[1] - p2[1];
    sum += delta*delta;
    delta = p1[2] - p2[2];
    sum += delta*delta;
    return(sum);
}

void 
vec_copypoints(int count, float *src, float *dest)
{
    int i;
    for(i=0;i<count;i++){
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest += 3;
        src += 3;
    }
}

void 
vec_zeropoints(int count, float *src)
{
    int i;
    for(i=0;i<count;i++){
        src[0] = 0.0;
        src[1] = 0.0;
        src[2] = 0.0;
        src += 3;
    }
}

void
mat_axisrotation(float *nvec, float radians, float (*m)[3])
{
    float c,s;
    float x,y,z,t;
    c = cos((double)radians);
    s = sin((double)radians);
    t = 1.0 - c;
    x = nvec[0];
    y = nvec[1];
    z = nvec[2];
    m[0][0] = t*x*x + c;
    m[0][1] = t*x*y + s*z;
    m[0][2] = t*x*z - s*y;
    m[1][0] = t*x*y - s*z;
    m[1][1] = t*y*y + c;
    m[1][2] = t*y*z + s*x;
    m[2][0] = t*x*z + s*y;
    m[2][1] = t*y*z - s*x;
    m[2][2] = t*z*z + c;
}

void
vec_align(float *p1, float *p2, float *p3, float *p4, float (*m)[3], float *t)
{
    /* set matrix m so that point p1 maps to p3 and p2 maps to p4 */
    /* p2 and p4 should be normalized */
    double dot,angle;
    float axis[3],tpoint[3];
    float axispoint[3];

    vec_sub(p3,p1,t);
    vec_cross(p2,p4,axis);
    vec_copy(p1,axispoint);
    dot = vec_dot(p2,p4);
    if(dot > 1.0){
        if(dot > 1.1)fprintf(stderr,"align: dot: %g\n",dot);
        dot = 1.0;
    }
    if(dot < -1.0){
        if(dot < -1.1)fprintf(stderr,"align: dot: %g\n",dot);
        dot = -1.0;
    }
    angle = acos(dot);
    angle = angle - M_PI;
    vec_normalize(axis);
    mat_axisrotation(axis,angle,m);
    vec_transform(axispoint,m,tpoint);
    vec_sub(axispoint,tpoint,axispoint);
    vec_add(t,axispoint,t);
}

int
vec_centroid(int count, float *src, float *p)
{
    float *ptr;
    int i;
    float sx,sy,sz;
    ptr = src;
    sx = sy = sz = 0.0;
    if(count == 0){
        fprintf(stderr,"centroid: zero points\n");
        p[0] = p[1] = p[2] = 0.0;
        return -1;
    }
    for(i = 0;i<count;i++){
        sx += ptr[0];
        sy += ptr[1];
        sz += ptr[2];
        ptr += 3;
    }
    sx /= count;
    sy /= count;
    sz /= count;
    p[0] = sx;
    p[1] = sy;
    p[2] = sz;
	return 0;
}

void
mat_smallrotmatrix(float *nvec, float angle, float (*m)[3])
{
    m[0][0] = 1.0;
    m[0][1] = nvec[2]*angle;
    m[0][2] = -nvec[1]*angle;

    m[1][0] = nvec[2]*angle;
    m[1][1] = 1.0;
    m[1][2] = nvec[0]*angle;

    m[2][0] = nvec[1]*angle;
    m[2][1] = -nvec[0]*angle;
    m[2][2] = 1.0;
}

void
mat_write(FILE *fp, float (*m)[3])
{
    fprintf(fp,"%g %g %g\n",m[0][0],m[0][1],m[0][2]);
    fprintf(fp,"%g %g %g\n",m[1][0],m[1][1],m[1][2]);
    fprintf(fp,"%g %g %g\n",m[2][0],m[2][1],m[2][2]);
}

void
vec_print(FILE *fp, float *p)
{
    fprintf(fp,"%.5f %.5f %.5f\n",p[0],p[1],p[2]);
}

void
vec_printpair(FILE *fp, float *p, float *n)
{
    fprintf(fp,"%.5f %.5f %.5f  %.5f %.5f %.5f\n",
        p[0],p[1],p[2],n[0],n[1],n[2]);
}

void
vec_mult(float *a, float *b, float *c)
{
    c[0] = a[0] * b[0];
    c[1] = a[1] * b[1];
    c[2] = a[2] * b[2];
}

void
vec_offset(float s, float *a, float *b, float *c)
{
    c[0] = a[0] + b[0]*s;
    c[1] = a[1] + b[1]*s;
    c[2] = a[2] + b[2]*s;
}

float
mat_det(float (*m)[3])
{
	return(
	   -m[0][2]*m[1][1]*m[2][0] + m[0][1]*m[1][2]*m[2][0] + m[0][2]*m[1][0]*m[2][1]
	   - m[0][0]*m[1][2]*m[2][1] - m[0][1]*m[1][0]*m[2][2] + m[0][0]*m[1][1]*m[2][2]);
}

int
mat_inverse(float (*m)[3], float (*minv)[3])
{
	double det,invdet;
	det = mat_det(m);
	if(fabs(det < VEC_FUZZ)) return -1;
	invdet = 1.0/det;

    minv[0][0] =  (m[1][1]*m[2][2] - m[1][2]*m[2][1])*invdet;
    minv[0][1] =  (m[0][2]*m[2][1] - m[0][1]*m[2][2])*invdet;
    minv[0][2] =  (m[0][1]*m[1][2] - m[0][2]*m[1][1])*invdet;
    minv[1][0] =  (m[1][2]*m[2][0] - m[1][0]*m[2][2])*invdet;
    minv[1][1] =  (m[0][0]*m[2][2] - m[0][2]*m[2][0])*invdet;
    minv[1][2] =  (m[0][2]*m[1][0] - m[0][0]*m[1][2])*invdet;
    minv[2][0] =  (m[1][0]*m[2][1] - m[1][1]*m[2][0])*invdet;
    minv[2][1] =  (m[0][1]*m[2][0] - m[0][0]*m[2][1])*invdet;
    minv[2][2] =  (m[0][0]*m[1][1] - m[0][1]*m[1][0])*invdet;
	return 0;
}

void
vec_rand(float *v)
{
	do {
		v[0] = VEC_RANDRANGE();
		v[1] = VEC_RANDRANGE();
		v[2] = VEC_RANDRANGE();
	} while (vec_magsq(v) < 1.0);
}

void
vec_average(float *p1, float *p2, float *p3, float *ave)
{
    ave[0] = p1[0] + p2[0] + p3[0];
    ave[1] = p1[1] + p2[1] + p3[1];
    ave[2] = p1[2] + p2[2] + p3[2];
    ave[0] *= VEC_ONETHIRD;
    ave[1] *= VEC_ONETHIRD;
    ave[2] *= VEC_ONETHIRD;
}

/* return maximum element of 3-vector */
float
vec_max(float *v)
{
    float max;
    max = v[0];
    if(v[1] > max) max = v[1];
    if(v[2] > max) max = v[2];
    return max;
}

/* coordinate transformations */
void
vec_stoc(float *a, float *b)
{
    b[0] = a[0]*sin((double)a[1])*cos((double)a[2]);
    b[1] = a[0]*sin((double)a[1])*sin((double)a[2]);
    b[2] = a[0]*cos((double)a[1]);
}

void
vec_ctos(float *p, float *q)
{
    double r,theta,phi;
    r = sqrt((double)(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]));
    if (r < .001) {
        q[0] = q[1] = q[2] = 0.0;
    } else {
        theta = acos((double)p[2]/r);
        if(VEC_FZEROP(p[1]) && VEC_FZEROP(p[0])) {
            phi = 0.0;
        } else {
            phi = atan2((double)p[1], (double)p[0]);
            if(phi < 0.0) phi += M_2PI;
        }

        q[0] = r;
        q[1] = theta;
        q[2] = phi;
    }
}

void
mat_read(FILE *fp, float (*m)[3], float *v)
{
    char buf[VEC_INBUFSIZE];

	/* init */
	vec_zero(v);
	mat_identity(m);

    /* read input */
    fgets(buf,sizeof(buf),fp);
    sscanf(buf,"%f %f %f %f",
        &m[0][0],&m[0][1],&m[0][2],&v[0]);
    fgets(buf,sizeof(buf),fp);
    sscanf(buf,"%f %f %f %f",
        &m[1][0],&m[1][1],&m[1][2],&v[1]);
    fgets(buf,sizeof(buf),fp);
    sscanf(buf,"%f %f %f %f",
        &m[2][0],&m[2][1],&m[2][2],&v[2]);
    if(fgets(buf,sizeof(buf),fp) != NULL){
        sscanf(buf,"%f %f %f",&v[0],&v[1],&v[2]);
    }
}

/*
 * mat_jacobi    Jacobi diagonalization of 3x3 symmetic matrix
 */

#define JACOBI_MAXOFFDIAG    1.0e-6  /* Maximum Off Diag Element for Jacobi */
#define JACOBI_MAXANGLE    0.25    /* Maximum angular error for Jacobi */
#define JACOBI_MAXITERATIONS 25

int
mat_jacobi(float (*matrix)[3], float *eigenvalues, float (*eigenvectors)[3])
{
    MATRIX33 a, u, w, r, tv;
    float sval, cval, diff;
    double angle;    /* rotation angle (RADIANS) */
    int x, y;
    int iteration;

    /* init */
    mat_copy(matrix,a);
    mat_identity(r);
    iteration = 0;

label1:

#ifdef VEC_DEBUG
    if(debug > 5) fprintf(stderr, "jacobi: iteration %d\n",iteration);
#endif
    /* find largest off diagonal element */
    if((fabs(a[0][1]) > fabs(a[0][2])) && (fabs(a[0][1]) > fabs(a[1][2]))){
        x = 0;
        y = 1;
    }
    else
        if(fabs(a[0][2])  > fabs(a[1][2])) {
            x = 0;
            y = 2;
        }
        else
        {
            x = 1;
            y = 2;
        }
#ifdef VEC_DEBUG
    if(debug > 5) fprintf(stderr, "jacobi: maxoffdiag = %g\n", a[x][y]);
#endif
    if(fabs(a[x][y]) < (double)JACOBI_MAXOFFDIAG) { /* done */
        eigenvalues[0] = a[0][0];
        eigenvalues[1] = a[1][1];
        eigenvalues[2] = a[2][2];
        mat_copy(r, eigenvectors);
        return 0;
    }
    /* compute rotation angle (RADIANS) */
    diff = a[x][x] - a[y][y];
    if(fabs((double)diff) > (double)1.e-12)
        angle = .5*atan(2.*a[x][y]/diff);
    else
        angle = M_PI/4.;
    /* check angle termination condition */
#ifdef VEC_DEBUG
    if(debug > 5) fprintf(stderr, "jacobi: angle = %g (degrees)\n", 
		VEC_RAD_TO_DEG(angle));
#endif
    /* PROBLEM HERE */
    if(fabs(angle) < (VEC_DEG_TO_RAD(JACOBI_MAXANGLE))){ /* done */
        eigenvalues[0] = a[0][0];
        eigenvalues[1] = a[1][1];
        eigenvalues[2] = a[2][2];
        mat_copy(r, eigenvectors);
        return 0;
    }
    cval = cos(angle);
    sval = sin(angle);
    mat_identity(tv);
    tv[x][x] = tv[y][y] = cval;
    tv[y][x] = sval;
    tv[x][y] = - sval;
    mat_copy(tv, u);
    u[x][y] = sval;
    u[y][x] = -sval;
    mat_mult(a, tv, w);
    mat_mult(u,w,a);
    mat_mult(r,tv,w);
    mat_copy(w,r);
    iteration++;
    if(iteration > JACOBI_MAXITERATIONS){
        eigenvalues[0] = a[0][0];
        eigenvalues[1] = a[1][1];
        eigenvalues[2] = a[2][2];
        mat_copy(r, eigenvectors);
        return -1;
    }
    goto label1;
}

void 
mat_to_quat (float (*m)[3], float *q)
{
	float w,wsq,tmp,ysq;
	float x,y,z,xsq;
	wsq	= .25*(1.0 + m[0][0] + m[1][1] + m[2][2]);
	if(wsq > VEC_FUZZ){
		w = sqrtf(wsq);
		tmp = .25/w;
		x = (m[1][2] - m[2][1])*tmp;
		y = (m[2][0] - m[0][2])*tmp;
		z = (m[0][1] - m[1][0])*tmp;
	} else {
	w = 0.0;
	xsq = .5*(m[1][1] + m[2][2]);
	if(xsq > VEC_FUZZ){
		x = sqrtf(xsq);
		tmp = .5/x;
		y = m[0][1]*tmp;
		z = m[0][2]*tmp;
	} else {
		x = 0.0;
		ysq = .5*(1.0 - m[2][2]);
		if(ysq > VEC_FUZZ){
			y = sqrtf(ysq);
			z = m[1][2]/(2.0*y);
		} else {
			y = 0.0;
			z = 1.0;
		}
	}
	}
		q[0] = x;
		q[1] = y;
		q[2] = z;
		q[3] = w;
	}

/* convert normalized quat to matrix */
void 
quat_to_mat (float *q, float (*m)[3])
{
    float w, x, y, z;
    float tx, ty, tz;
    float twx, txx, txy, txz;
    float twy,      tyy, tyz;
    float twz,           tzz;

	x = q[0];
	y = q[1];
	z = q[2];
	w = q[3];

    tx  = x+x;
    ty  = y+y;
    tz  = z+z;

    twx = w*tx;
    txx = x*tx;
    txy = y*tx;
    txz = z*tx;

    twy = w*ty;
    tyy = y*ty;
    tyz = z*ty;

    twz = w*tz;
    tzz = z*tz;

    m[0][0] = 1. - tyy - tzz;
    m[0][1] =      txy + twz;
    m[0][2] =      txz - twy;
    m[1][0] =      txy - twz;
    m[1][1] = 1. - txx - tzz;
    m[1][2] =      tyz + twx;
    m[2][0] =      txz + twy;
    m[2][1] =      tyz - twx;
    m[2][2] = 1. - txx - tyy;
	}

void
quat_normalize(float *q)
{
	float mag;
    mag = sqrtf((float)(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]));
	mag = 1.0/mag;
    q[0] *= mag;
    q[1] *= mag;
    q[2] *= mag;
    q[3] *= mag;
}

/* eof */
