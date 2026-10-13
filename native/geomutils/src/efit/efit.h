/* efit.h */

/* defines */
#define MAXPOINTS 150*1024
#define MATRIXFORMAT    "%14.7g %14.7g %14.7g\n%14.7g %14.7g %14.7g\n%14.7g %14.7g %14.7g\n"
#define POSITIONFORMAT    "%14.7g %14.7g %14.7g\n"
#define EFIT_NAME_LENGTH 64
#define INBUFSIZE 256

#define X 0
#define Y 1
#define Z 2

#define EFIT_OK 0
#define EFIT_ERROR -1

typedef float    matrix33[3][3];

/* SYSV def */
#define index strchr

/* MACROS */
#define RAD_TO_DEG(x) ((x)*(180.0/M_PI))
#define DEG_TO_RAD(x) ((x)*(M_PI/180.0))

/* STRUCTURES */
struct datum {
    float point[3];
    float weight;
};

struct ellipsoid {
    char     name[EFIT_NAME_LENGTH];
    float    position[3];        /* center of mass */
    double   axis[3];        /* major, minor, middle axis lengths */
    float    orientation[3][3];    /* orientation matrix */
    float    inv_orientation[3][3];/* inverse orientation matrix */
    float    tensor[3][3];            /* inertia or covariance */
};

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

#include "efit_globals.h"

/* end of file */
