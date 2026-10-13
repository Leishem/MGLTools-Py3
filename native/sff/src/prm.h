/* MS modified to work with sff.c */

#ifndef NO_DOUBLE
#define DOUBLE
#endif

#ifdef DOUBLE
typedef double _REAL;
#define UseDouble 1
#else
typedef float _REAL;
#define UseDouble 0
#endif

typedef struct parm {
	int 	IfBox, Nmxrs, IfCap,
		 Natom,  Ntypes,  Nbonh,  Mbona,  Ntheth,  Mtheta, 
		 Nphih,  Mphia,  Nhparm, Nparm, Nnb, Nres,
		 Nbona,  Ntheta,  Nphia,  Numbnd,  Numang,  Nptra,
		 Natyp,  Nphb, Nat3, Ntype2d, Nttyp, Nspm, Iptres, Nspsol,
		 Ipatm, Natcap;
	char 	*ititl, *AtomNames, *ResNames, *AtomSym, *AtomTree;
	_REAL	*Charges, *Masses, *Rk, *Req, *Tk, *Teq, *Pk, *Pn, *Phase,
		 *Solty, *Cn1, *Cn2, *HB12, *HB10;
	_REAL	Box[3], Cutcap, Xcap, Ycap, Zcap;
	int 	*Iac, *Iblo, *Cno, *Ipres, *ExclAt, *TreeJoin, 
		*AtomRes, *BondHAt1, *BondHAt2, *BondHNum, *BondAt1, *BondAt2, 
		*BondNum, *AngleHAt1, *AngleHAt2, *AngleHAt3, *AngleHNum, 
		*AngleAt1, *AngleAt2, *AngleAt3, *AngleNum, *DihHAt1, 
		*DihHAt2, *DihHAt3, *DihHAt4, *DihHNum, *DihAt1, *DihAt2, 
		*DihAt3, *DihAt4, *DihNum, *Boundary;
	int	*N14pairs, *N14pairlist;
} parmstruct;

typedef struct SFFoptions
{
  _REAL cut;                  /* non-bonded cutoff (8.0)*/
  _REAL scnb;                 /* scale factor for 1-4 nonbonds (2.0) */
  _REAL scee;                 /* scale factor for 1-4 electrostatics (1.2) */
  int  ntpr;                  /*  print frequency  (10)*/
  int  nsnb;                  /*  non-bonded update frequency (25) */
  int  mme_init_first;        /* 1 */

  int  *frozen;
  int  nfrozen;

  int  *constrained ;
  int  nconstrained ;
  _REAL *x0 ;                /*  will hold coordinates to be constrained  */
  _REAL wcons ;              /*  weight of constraints    */


  int *npairs;
  int *pairlist;
  int maxnb;
  int dield;             /* dielectric function to be used */
  _REAL w4d ;
  int dim;           /* dimension: 3 or 4 */
  

                            /*  MD variables:  */

  _REAL t;		    /* initial time (0)*/
  _REAL dt;	            /* time step, ps.(0.001) */
  _REAL tautp;	            /* temp. coupling parm., ps (0.02) */
  _REAL temp0;	            /* target temperature, K (300.0)*/
  _REAL boltz2;             /* 9.93595e-4 */
  _REAL vlimit;	            /* maximum velocity component (10.0)*/
  int  ntpr_md ;	    /* print frequency  (10)*/
  int  ntwx;		    /* trajectory snapshot frequency (0) */
  FILE *binposfp;    /* file pointer for trajectories  */
  int  zerov;		    /* if true, use zero initial velocities (0)*/
  _REAL tempi;              /* initial temperature (0.0)*/
  int  idum;                /* random number seed (-1)*/
  _REAL enbr, ehbr, eelr, etorr, enb14r, eel14r; /* energies of monitors residue */
  int  nhbpair;
} SFFoptions;

SFFoptions *init_sff_options(void);
parmstruct *readparm(char *name);
int readcrd(char *name, _REAL ***c, parmstruct *parm);
_REAL **readcrdvec(char *name, parmstruct *parm, int *nat);

void mme_initCallbacks();
int mme_init(int *froz, int *constr, _REAL *x0i, FILE *bfpi,
 	     parmstruct *prm, SFFoptions * opts);
void  mme_cleanup();

typedef _REAL (*mme_f)( _REAL *x, _REAL *f, int *iter, _REAL *ene, parmstruct *prm , SFFoptions *opts);

_REAL mme( _REAL *x, _REAL *f, int *iter, _REAL *ene, parmstruct *prm, SFFoptions *opts );

int conjgrad( _REAL *x, int *n, _REAL *f, mme_f func,
	      _REAL *dgrad, _REAL *dfpred, int *maxiter, parmstruct *prm,
	      _REAL *ene, SFFoptions *opts);

int md( int n, int maxstep, _REAL *x, _REAL *minv, _REAL *f, _REAL *v,
	mme_f func, _REAL *ene, parmstruct *prm, SFFoptions *opts );
int mm_options( char *c, float val, SFFoptions *opts );
int md_options( char *c, float val, SFFoptions *opts );

void sffC_list_options(SFFoptions *opts);

int startbinpos( FILE *fp );
int openbinpos( FILE * fp );
int readbinpos( int n_atom, _REAL *apos, FILE *fp );
int writebinpos(int n_atom, _REAL *apos, FILE *fp );

/* mme options */
/* extern _REAL  cut, scnb, scee, wcons; */
/* extern int nsnb, ntpr, mme_init_first, dield, verbosemm, stop_flag; */

/* md options */
/* extern _REAL  t, dt, tautp, temp0, boltz2, vlimit, tempi; */
/* extern int ntpr_md, ntwx, zerov, idum, verbosemd; */
extern int verbosemd, verbosemm;
extern int stop_flag;

extern void sanityCb(int nbat, int cbNum, _REAL *coords, _REAL *energies, int step);

/* extern int  *frozen, nfrozen; */
/* extern int  *constrained, nconstrained; */
/* extern _REAL *x0;  */        /*  will hold coordinates to be constrained  */

typedef void (*sffcb_f)(int cbNum, int nbat, _REAL *coords, _REAL *energies, int step);

struct cbFunc {
  sffcb_f fun;
  int freq;
};

typedef struct cbFunc cbFunc_t;

/* number of callback calling point in mme inner loop */
#define NCBFUNC 2

/* array of C function call at callback calling points */
extern cbFunc_t mme_callback[NCBFUNC];

void setccallback( sffcb_f cbfun, int frequency, int callbacknum);
