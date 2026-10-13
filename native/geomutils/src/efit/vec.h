/* vec.h */

#ifndef __VEC_H_
#define __VEC_H_ 1

#include <math.h>

#ifndef MAXFLOAT
#define MAXFLOAT            ((float)3.40282346638528860e+38)
#endif

#ifndef M_PI
#define    M_PI    3.14159265358979323846
#endif

#ifndef M_2PI
#define    M_2PI    (2.0*M_PI)
#endif

#define VEC_FUZZ 1.0e-8
#define VEC_INBUFSIZE 128

#ifndef VEC_FZEROP
#define VEC_FZEROP(x)	((x < VEC_FUZZ) && (x > -VEC_FUZZ))
#endif

#ifdef NO_DRAND48
#define RANDFLOAT_MASK 03777777             /* mask for random function */
#define VEC_SRANDFLOAT(x)    srandom(x)
#define VEC_RANDFLOAT() ((random() & RANDFLOAT_MASK)/(float)(RANDFLOAT_MASK))
#else
double drand48(void);
void srand48(long int);
#define VEC_RANDFLOAT() drand48()
#define VEC_SRANDFLOAT(x)    srand48(x)
#endif /* NO_DRAND48 */

#define VEC_SQRT(x) (sqrt((double)(x)))
#define VEC_RANDRANGE() (2.0*(VEC_RANDFLOAT() - .5))
#define VEC_ONETHIRD (float).3333333

/** typedef float matrix33[3][3]; **/
typedef float MATRIX33[3][3];

/* vector/matrix  FUNCTIONS */

int vec_normalize(float *p);
int vec_centroid(int count, float *src, float *p);

float vec_dot(float *p1, float *p2);
float vec_magsq(float *p);
float vec_mag(float *p);
float vec_distancesq(float *p1, float *p2);
float vec_distance(float *p1, float *p2);
float vec_max(float *v);
float vec_length(float *p);

void vec_ctos(float *p, float *q);
void vec_stoc(float *a, float *b);
void vec_sub(float *p1, float *p2, float *p);
void vec_copy(float *p1, float *p2);
void vec_add(float *p1, float *p2, float *p);
void vec_scale(float f, float *p1, float *p2);
void vec_zero(float *p);
void vec_zeropoints(int count, float *src);
void vec_cross(float *a, float *b, float *c);
void vec_align(float *p1, float *p2, float *p3, float *p4, float (*m)[3], float *t);
void vec_mult(float *a, float *b, float *c);
void vec_offset(float s, float *a, float *b, float *c);
void vec_rand(float *v);
void vec_average(float *p1, float *p2, float *p3, float *ave);
void vec_copypoints(int count, float *src, float *dest);
void vec_print(FILE *fp, float *p);
void vec_printpair(FILE *fp, float *p, float *n);
void vec_zeropoints(int count, float *src);
void vec_transform(float *p, float (*m)[3], float *q);
void vec_ftransform(float *p, float (*m)[3], float *q);

int mat_inverse(float (*m)[3], float (*minv)[3]);

float mat_det(float (*m)[3]);
void mat_smallrotmatrix(float *nvec, float angle, float (*m)[3]);
void mat_axisrotation(float *nvec, float radians, float (*m)[3]);
void mat_read(FILE *fp, float (*m)[3], float *v);
void mat_write(FILE *fp, float (*m)[3]);
void mat_identity(float (*m)[3]);
void mat_copy(float (*m1)[3], float (*m2)[3]);
void mat_transpose(float (*m1)[3], float (*m2)[3]);
void mat_mult(float (*m1)[3], float (*m2)[3], float (*m)[3]);
void mat_copy(float (*m1)[3], float (*m2)[3]);
void quat_to_mat (float *q, float (*m)[3]);
void mat_to_quat (float (*m)[3], float *q);

#endif

/* end of file */
