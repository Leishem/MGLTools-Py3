
%module efitlib
%feature("python:cdefaultargs");
%include numarr.i

%init %{
	import_array(); /* load the Numpy PyCObjects */
%}

%{
#include "efit.h"
#define EFIT_NAME_LENGTH 64
#include "vec.h"
%}

struct ellipsoid {
    char     name[EFIT_NAME_LENGTH];
    float    position[3];        /* center of mass */
    double   axis[3];        /* major, minor, middle axis lengths */
    float    orientation[3][3];    /* orientation matrix */
    float    inv_orientation[3][3];/* inverse orientation matrix */
    float    tensor[3][3];            /* inertia or covariance */
};

%extend ellipsoid {
  void getPosition( float OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->position[0];
    OUT_VECTOR[1] = self->position[1];
    OUT_VECTOR[2] = self->position[2];
  }

  void getAxis( double OUT_VECTOR[3] ) {
    OUT_VECTOR[0] = self->axis[0];
    OUT_VECTOR[1] = self->axis[1];
    OUT_VECTOR[2] = self->axis[2];
  }

  void getOrientation( float OUT_ARRAY2D[3][3] ) {
    OUT_ARRAY2D[0][0] = self->orientation[0][0];
    OUT_ARRAY2D[0][1] = self->orientation[0][1];
    OUT_ARRAY2D[0][2] = self->orientation[0][2];
    OUT_ARRAY2D[1][0] = self->orientation[1][0];
    OUT_ARRAY2D[1][1] = self->orientation[1][1];
    OUT_ARRAY2D[1][2] = self->orientation[1][2];
    OUT_ARRAY2D[2][0] = self->orientation[2][0];
    OUT_ARRAY2D[2][1] = self->orientation[2][1];
    OUT_ARRAY2D[2][2] = self->orientation[2][2];
  }
}

struct efit_info {
    int weightflag;
    int covarflag;
    int volumeflag;
    int matrixflag;
    int nocenterflag;
    int noscaleflag;
    int nosortflag;
    int count;
    float cov_scale;        /* multiplier for covariance std axies */
    float ell_scale;      /* multiplier for ellipsoid axies */
};


%typemap(in) (float *pts, int nbpts) {
  int expected_dims[2] = {0,3};
  PyArrayObject *array;
  array = contiguous_typed_array($input, PyArray_FLOAT, 2, expected_dims);
  if (!array) return NULL;
  $1 = (float(*))array->data;
  $2 = ((PyArrayObject *)(array))->dimensions[0];
}

extern int fitEllipse(float *pts, int nbpts, float ell_scale, float cov_scale,
		      struct efit_info *eptr, struct ellipsoid *ellipsoid);

%typemap(in) float vector_in_out[ANY] (PyArrayObject *array, int expected_dims[1])
%{
  expected_dims[0] = $1_dim0;
  if (expected_dims[0]==1) expected_dims[0]=0;
  array = contiguous_typed_array($input, PyArray_FLOAT, 1, expected_dims);
  if (! array) return NULL;
  $1 = (float *)array->data;
%}

%typemap (argout) float vector_in_out[ANY] (int i)
%{
  $result = PyList_New($1_dim0);
  for (i = 0; i < $1_dim0; i++) {
      PyObject *o = PyFloat_FromDouble((double) $1[i]);
      PyList_SetItem($result,i,o);
    }
   Py_DECREF((PyObject *)array$argnum);
%}
/**
%typemap(freearg) float vector_in_out[ANY]

%{
  if ( array$argnum )
      Py_DECREF((PyObject *)array$argnum);
   
%}
**/

int vec_normalize(float vector_in_out[3]);
int vec_centroid(int count, float *src, float *p);

float vec_dot(float VECTOR[3], float VECTOR[3]);
float vec_magsq(float VECTOR[3]);
float vec_mag(float VECTOR[3]);
float vec_distancesq(float VECTOR[3], float VECTOR[3]);
float vec_distance(float VECTOR[3], float VECTOR[3]);
float vec_max(float VECTOR[3]);
float vec_length(float VECTOR[3]);

void vec_ctos(float VECTOR[3], float OUT_VECTOR[3]);
void vec_stoc(float VECTOR[3], float OUT_VECTOR[3]);
void vec_sub(float VECTOR[3], float VECTOR[3], float OUT_VECTOR[3]);
void vec_copy(float VECTOR[3], float OUT_VECTOR[3]);
void vec_add(float VECTOR[3], float VECTOR[3], float OUT_VECTOR[3]);
void vec_scale(float f, float VECTOR[3], float OUT_VECTOR[3]);
void vec_zero(float OUT_VECTOR[3]);
void vec_cross(float VECTOR[3], float VECTOR[3], float OUT_VECTOR[3]);
/**void vec_align(float *p1, float *p2, float *p3, float *p4, float (*m)[3], float *t); **/
void vec_mult(float VECTOR[3], float VECTOR[3], float OUT_VECTOR[3]);
void vec_offset(float s, float VECTOR[3], float VECTOR[3], float OUT_VECTOR[3]);
void vec_rand(float OUT_VECTOR[3]);
void vec_average(float VECTOR[3], float VECTOR[3], float VECTOR[3], float OUT_VECTOR[3]);
/** void vec_copypoints(int count, float *src, float *dest); **/
/** void vec_print(FILE *fp, float *p); **/
/** void vec_printpair(FILE *fp, float *p, float *n); **/
void vec_transform(float VECTOR[3], float ARRAY2D[3][3], float OUT_VECTOR[3]);
void vec_ftransform(float VECTOR[3], float ARRAY2D[3][3], float OUT_VECTOR[3]);

/** int mat_inverse(float (*m)[3], float (*minv)[3]); **/

/** float mat_det(float (*m)[3]); **/
/** void mat_smallrotmatrix(float *nvec, float angle, float (*m)[3]); **/
/** void mat_axisrotation(float *nvec, float radians, float (*m)[3]); **/
/** void mat_read(FILE *fp, float (*m)[3], float *v); **/
/** void mat_write(FILE *fp, float (*m)[3]); **/

/** void mat_identity(float (*m)[3]); **/
/** void mat_copy(float (*m1)[3], float (*m2)[3]); **/
/** void mat_transpose(float (*m1)[3], float (*m2)[3]); **/
/** void mat_mult(float (*m1)[3], float (*m2)[3], float (*m)[3]); **/

int mat_jacobi(float ARRAY2D[3][3], float OUT_VECTOR[3], float OUT_ARRAY2D[3][3]);
void quat_to_mat (float VECTOR[4], float OUT_ARRAY2D[3][3]);
void mat_to_quat (float ARRAY2D[3][3], float VECTOR[4]);
