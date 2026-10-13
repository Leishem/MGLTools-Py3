/*
 *
 * This software is copyrighted, 1995, by Tom Macke and David A. Case. 
 * The following terms apply to all files associated with the software 
 * unless explicitly disclaimed in individual files.
 * 
 * The authors hereby grant permission to use, copy, modify, and re-distribute
 * this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be distributed provided that
 * the nature of the modifications are clearly indicated.
 * 
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
 * DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
 * IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
 * NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 * 
 */

/*  sff.c:  simple force field: implement routines to read a "prmtop"
 *      file from AMBER, and calculate the energy.  Implements only
 *      some of AMBER's functionality: bonds, angles, dihedrals, and
 *      nonbonded interactions with a distance-dependent dielectric.
 *      Does not (yet) include support for period systems.
 *
 *      Main interface is through routines "mme" and "mme_init".
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "prm.h"
#include "memutil.h"
#include "assert.h"
#include "bhtree.h"

#ifdef GRAPHICS
static int  update_display=0;
#endif
static int  nhbpair;
static FILE *tmp;
int verbosemm=0, verbosemd=0;
/*int debres=-1, endres=-1;     residue to monitor */

/* _REAL enbrtmp, ehbrtmp, eelrtmp, etorrtmp; */
                           /*  general force field variables   */

#ifdef flex
static char *mmoinputptr;
static int mmoinputlim;
#endif

#ifdef __APPLE__
#include <sys/time.h>
_REAL   second()
{
  struct timeval t;
  struct timezone tz;
  gettimeofday(&t,&tz);
  return (double)t.tv_sec + (double)t.tv_usec;
}
#else
#include <sys/timeb.h>

_REAL	second()
{
  struct timeb t;
  ftime(&t);
  return (double)t.time + (double)t.millitm * (double)0.001;
}
#endif
static	_REAL	t1, t2, tnonb, tpair, tbond, tangl, tphi, tcons;
							/* timer variables  */
  /* _REAL *resRad=NULL;  residue radii array, used in nblist */


float  gauss();	/* never double -bsd- */
  
#ifdef WIN32
#define strncasecmp strncmp
#endif

/* bsd mods 11 Jun */
/* signals */

#define SFF_OK 0
#define SFF_ERROR -1
int stop_flag = 0;
#include <signal.h>

#define DOT(a,b,c,d,e,f) a*d + b*e + c*f

void sff_catcher(int signo)
{
	/* reset the signal */
	signal(signo,sff_catcher);

	if(verbosemm)fprintf(stderr,"sff_catcher: caught signal %d\n",signo);

	switch(signo){
	case SIGINT:
		stop_flag = 1;
		break;
#ifndef WIN32
	case SIGUSR1:
		break;
	case SIGHUP:
		break;
	case SIGQUIT:
		break;
#endif
	default:
		break;
	}
	fflush(stdout);
	fflush(stderr);
}

int sff_init_signals(void){
	assert(sff_catcher);
	signal(SIGINT,sff_catcher);
	return(SFF_OK);
}

int sff_reset_signals(void)
{
	signal(SIGINT,SIG_DFL);
	return(SFF_OK);
}

SFFoptions *init_sff_options(void)
{
  SFFoptions * opts;
  opts = (SFFoptions *)malloc(sizeof(SFFoptions));
  if( !opts )
    nrerror( "allocation failure in init_sff_options()" );
  opts->cut = 8.0;
  opts->scnb = 2.0;
  opts->scee = 1.2;
  opts->ntpr = 10;  
  opts->nsnb = 25;  
  opts->mme_init_first = 1;
  
  opts->frozen = NULL;
  opts->nfrozen = 0;
  
  opts->constrained = NULL;
  opts->nconstrained = 0;
  opts->x0 = NULL; 
  opts->wcons = 0.0; 
  opts->dield = 0;	
  opts->w4d = 0.0;
  /*  MD variables:  */
  opts->t = 0.;
  opts->dt = 0.001;
  opts->tautp = 0.2;
  opts->temp0 = 300.;
  opts->boltz2 = 9.93595e-4;
  opts->vlimit = 10.0;
  opts->ntpr_md = 10;
  opts-> ntwx = 0;	
  opts->zerov = 0;	
  opts->tempi = 0.0;
  opts->idum = -1;   
  return opts;
}

void sffC_list_options(SFFoptions *opts)
{
  fprintf(stdout, "non-bonded cutoff                (cut), %f\n", opts->cut);                  
  fprintf(stdout, "scale factor for 1-4 nonbond    (scnb), %f\n", opts->scnb);
  fprintf(stdout, "scale factor for 1-4 electro.   (scee), %f\n", opts->scee);
  fprintf(stdout, "weight of constraints          (wcons), %f\n", opts->wcons);
  fprintf(stdout, "                      (mme_init_first), %d\n", opts->mme_init_first);
  fprintf(stdout, "dielectric function to be used (dield), %d\n", opts->dield);
  fprintf(stdout, "output level                 (verbose), %d\n", verbosemm);
  fprintf(stdout, "print frequency                 (ntpr), %d\n", opts->ntpr);
  fprintf(stdout, "non-bonded update frequency     (nsnb), %d\n", opts->nsnb);
  fprintf(stdout, "\nMD options -------------------------------------------\n");
  fprintf(stdout, "initial time                             (t), %f\n", opts->t);
  fprintf(stdout, "time step, ps.                          (dt), %f\n", opts->dt);
  fprintf(stdout, "temp. coupling parm., ps             (tautp), %f\n", opts->tautp);
  fprintf(stdout, "target temperature, K                (temp0), %f\n", opts->temp0);
  fprintf(stdout, "                                    (boltz2), %f\n", opts->boltz2);
  fprintf(stdout, "maximum velocity component          (vlimit), %f\n", opts->vlimit);
  fprintf(stdout, "print frequency                    (ntpr_md), %d\n", opts->ntpr_md);
  fprintf(stdout, "trajectory snapshot frequency         (ntwx), %d\n", opts->ntwx);
  fprintf(stdout, "if true, use zero initial velocities (zerov), %d\n", opts->zerov);
  fprintf(stdout, "initial temperature                  (tempi), %f\n", opts->tempi);
  fprintf(stdout, "random number seed                    (idum), %d\n",  opts->idum);
}


int mm_options( char *c, float val, SFFoptions *opts )
{
  if (!c) return;

  if (strncasecmp(c,"cut",strlen(c))==0)
    opts->cut = val;
  else if (strncasecmp(c,"ntpr",strlen(c))==0) {/* print frequency  */
    assert(val != 0);
    opts->ntpr = (int)val;
  } else if (strncasecmp(c,"nsnb",strlen(c))==0) /* non-bonded update frequency */
    opts->nsnb = (int)val;
  else if (strncasecmp(c,"scnb",strlen(c))==0) /* scale factor for 1-4 nonbond*/
    opts->scnb = val;
  else if (strncasecmp(c,"scee",strlen(c))==0)/* scale factor for 1-4 electro */
    opts->scee = val;
  else if (strncasecmp(c,"mme_init_first",strlen(c))==0)
    opts->mme_init_first = (int)val;
  else if (strncasecmp(c,"dield",strlen(c))==0) /* dielectric function */
    opts->dield = (int)val;
  else if (strncasecmp(c,"verbose",strlen("verbose"))==0) /*  */
    verbosemm = (int)val;
  else if (strncasecmp(c,"stop_flag",strlen(c))==0) /*  bsd */
    stop_flag = (int)val;

#ifdef MONITOR_RESIDUE
    } else if (strncasecmp(c,"residue",strlen(c))==0) { /* residue to monitor */
      i = get_intArg(c,verbosemm);
      if (i==-1) {
	debres = endres = -1;
      } else {
	debres = prm->Ipres[i]-1;
	endres = prm->Ipres[i+1]-1;
      }
    }
#endif
  else
    printf("ERROR: %s unknown parameter\n", c);
}

int md_options( char *c, float val, SFFoptions *opts )
{
  if (!c) return;

  if (strncasecmp(c,"t",strlen(c))==0) /* initial time */
   opts->t = val;
  else if (strncasecmp(c,"dt",strlen(c))==0) /* time step, ps.  */
   opts->dt = val;
  else if (strncasecmp(c,"tautp",strlen(c))==0) /* temp. coupling parm., ps */
    opts->tautp = val;
  else if (strncasecmp(c,"temp0",strlen(c))==0) /* target temperature, K */
    opts->temp0 = val;
  else if (strncasecmp(c,"boltz2",strlen(c))==0)/* boltz2 */
    opts->boltz2 = val;
  else if (strncasecmp(c,"vlimit",strlen(c))==0) /* maximum velocity component */
    opts->vlimit = val;
  else if (strncasecmp(c,"ntpr_md",strlen(c))==0) {/* print frequency */ 
    assert(val != 0);
    opts->ntpr_md = (int)val;
  } else if (strncasecmp(c,"zerov",strlen(c))==0) /* if true, use zero initial velocities */
    opts->zerov = (int)val;
  else if (strncasecmp(c,"tempi",strlen(c))==0) /* initial temperature */
    opts->tempi = (int)val;
  else if (strncasecmp(c,"idum",strlen(c))==0) /* random number seed */
    opts->idum = (int)val;
 else if (strncasecmp(c,"ntwx",strlen(c))==0) /* random number seed */
    opts->ntwx = (int)val;
  else if (strncasecmp(c,"verbose",strlen("verbose"))==0) /* random number seed */
    verbosemd = (int)val;
  else
    printf("ERROR: %s unknown parameter\n", c);
}


void  mme_cleanup(SFFoptions *opts)
{
   free(opts->frozen);
   opts->nfrozen = 0;
   free(opts->constrained);
   opts->nconstrained = 0;
   free(opts->npairs);
   free (opts);
}


void mme_initCallbacks()
{
  int i;

  for (i=0; i<NCBFUNC; i++)
    {
      mme_callback[i].fun = NULL;
      mme_callback[i].freq = 0;
    }
}


int   mme_init( froz, constr, x0i, bfpi, prm, opts )

int		*froz, *constr;
_REAL		*x0i;
FILE		*bfpi;
parmstruct      *prm;
SFFoptions      *opts;

{
  int i, j, ip1, ip2, nblimit;
  _REAL rrad;
  opts->dim = 3;

	opts->x0 = x0i;
	opts->binposfp = bfpi;

/*  	resRad = Rvector( 0, prm->Natom ); */

/*  	for (i=0; i<prm->Nres; i++) { */
/*  	  if (strncmp(&prm->ResNames[i*4], "ARG", 3)==0) rrad= 10.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "TRP", 3)==0) rrad= 10.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "TYR", 3)==0) rrad= 9.5; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "GLU", 3)==0) rrad= 9.5; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "GLH", 3)==0) rrad= 9.5; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "GLN", 3)==0) rrad= 9.5; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "LYS", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "LYN", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "HIE", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "HID", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "HIP", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "ASP", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "ASH", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "ASN", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "PHE", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "MET", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "LEU", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "ILE", 3)==0) rrad= 9.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "CYS", 3)==0) rrad= 8.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "CYM", 3)==0) rrad= 8.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "THR", 3)==0) rrad= 8.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "SER", 3)==0) rrad= 8.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "VAL", 3)==0) rrad= 8.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "PRO", 3)==0) rrad= 8.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "ALA", 3)==0) rrad= 7.0; */
/*  	  else if (strncmp(&prm->ResNames[i*4], "GLY", 3)==0) rrad= 6.0; */
/*  	  else { */
/*  	    printf("WARNING: residue %4s has no radius\n", &prm->ResNames[i*4]); */
/*  	    rrad = 100.; */
/*  	      } */
/*  	  ip1 = prm->Ipres[i]; */
/*  	  ip2 = prm->Ipres[i+1] - 1; */
/*  	  for (j=ip1-1; j<ip2; j++) */
/*  	    resRad[j] = rrad; */
/*  	} */
	/*	free_vector( resRad, 0, prm->Natom ); */

	if( opts->mme_init_first ){
		opts->frozen = ivector( 0, prm->Natom );
		opts->constrained = ivector( 0, prm->Natom );
		opts->npairs = ivector( 0, prm->Natom );

				/* crude estimate for the number of non-bonded pairs */
		opts->maxnb = prm->Natom * opts->cut * opts->cut * opts->cut / 1.25;
		nblimit = prm->Natom*prm->Natom/2 - prm->Natom;
		if (opts->maxnb > nblimit) opts->maxnb = nblimit;
#ifdef DEBUG
		printf( "allocating space for %d non-bonded pairs\n", opts->maxnb ); 
#endif
		opts->pairlist = ivector( 0, opts->maxnb );
		opts->mme_init_first = 0;

		opts->nfrozen = 0;
		opts->nconstrained = 0;
		for (i=0; i<prm->Natom; i++) {
		  opts->frozen[i] = opts->constrained[i] = 0;
		}
	}

        if (froz) {
	  opts->nfrozen = 0;
	  for (i=0; i<prm->Natom; i++) {
	    if (froz[i]) {
	      opts->frozen[i] = 1;
	      opts->nfrozen++;
	    } else opts->frozen[i] = 0;
	  }
	  printf( "froze %d atoms\n", opts->nfrozen );
	} else {
	  for (i=0; i<prm->Natom; i++) {
	    opts->frozen[i] = 0;
	  }
	}

        if (constr) {
	  opts->nconstrained = 0;
	  for (i=0; i<prm->Natom; i++) {
	    if (constr[i]) {
	      opts->constrained[i] = 1;
	      opts->nconstrained++;
	    } else opts->constrained[i] = 0;
	  }
	  printf( "constrained %d atoms\n", opts->nconstrained );
	} else {
	  for (i=0; i<prm->Natom; i++) {
	    opts->constrained[i] = 0;
	  }
	}


#ifdef ORIG	
	opts->nfrozen = set_belly_mask( m, aexp, opts->frozen );
	opts->nconstrained = set_cons_mask( m, aexp2, opts->constrained );
#endif
	return( 0 );
}

_REAL econs( x, f, prm, opts )
_REAL   *x, *f;
parmstruct      *prm;
SFFoptions *opts;
{
	int i;
	_REAL  e_cons,rx,ry,rz;

	e_cons = 0.0;
	for( i=0; i<prm->Natom; i++ ){
		if( opts->constrained[i] ){
			rx = x[3*i    ] - opts->x0[3*i    ];
			ry = x[3*i + 1] - opts->x0[3*i + 1];
			rz = x[3*i + 2] - opts->x0[3*i + 2];
			e_cons += opts->wcons*(rx*rx + ry*ry + rz*rz);
			f[3*i    ] += 2.*opts->wcons*rx;
			f[3*i + 1] += 2.*opts->wcons*ry;
			f[3*i + 2] += 2.*opts->wcons*rz;
		}
	}
    return( e_cons );
}

_REAL ebond( nbond, a1, a2, atype, Rk, Req, x, f )
int		nbond, *a1, *a2, *atype;
_REAL   *Rk, *Req, *x, *f;
{
	int i,at1,at2,atyp;
	_REAL  e_bond,r,rx,ry,rz,r2,s,db,df,e;

	e_bond = 0.0;
	for( i=0; i<nbond; i++ ){
		at1 = a1[i]; at2 = a2[i]; atyp = atype[i] - 1;
		rx = x[at1    ] - x[at2    ];
		ry = x[at1 + 1] - x[at2 + 1];
		rz = x[at1 + 2] - x[at2 + 2];
		r2 = rx*rx + ry*ry + rz*rz;
		s = sqrt(r2);
		r = 2.0/s;
		db =  s - Req[atyp];
		df = Rk[atyp] * db;
		e = df*db;
		e_bond += e;
		df *= r;
		f[at1 + 0] += rx*df;
		f[at1 + 1] += ry*df;
		f[at1 + 2] += rz*df;
		f[at2 + 0] -= rx*df;
		f[at2 + 1] -= ry*df;
		f[at2 + 2] -= rz*df;
	}
    return( e_bond );
}

_REAL eangl( nang, a1, a2, a3, atype, Tk, Teq, x, f )
int		nang, *a1, *a2, *a3, *atype;
_REAL   *Tk, *Teq, *x, *f;
{
	int i,atyp,at1,at2,at3;
	_REAL  dxi,dyi,dzi,dxj,dyj,dzj,ri2,rj2,ri,rj,rir,rjr;
	_REAL  dxir,dyir,dzir,dxjr,dyjr,dzjr,cst,at,da,df,e,e_theta;
	_REAL  xtmp,dxtmp,ytmp,dytmp,ztmp,dztmp;

	e_theta = 0.0;
	for( i=0; i<nang; i++ ){
		at1 = a1[i]; at2 = a2[i]; at3 = a3[i]; atyp = atype[i] - 1;

		dxi = x[at1    ] - x[at2    ];
		dyi = x[at1 + 1] - x[at2 + 1];
		dzi = x[at1 + 2] - x[at2 + 2];
		dxj = x[at3    ] - x[at2    ];
		dyj = x[at3 + 1] - x[at2 + 1];
		dzj = x[at3 + 2] - x[at2 + 2];

		ri2=dxi*dxi+dyi*dyi+dzi*dzi;
		rj2=dxj*dxj+dyj*dyj+dzj*dzj;
		ri=sqrt(ri2);
		rj=sqrt(rj2);
		rir=1./ri;
		rjr=1./rj;

		dxir=dxi*rir;
		dyir=dyi*rir;
		dzir=dzi*rir;
		dxjr=dxj*rjr;
		dyjr=dyj*rjr;
		dzjr=dzj*rjr;

		cst=dxir*dxjr+dyir*dyjr+dzir*dzjr;
		if (cst > 1.0) cst = 1.0;
		if (cst < -1.0) cst = -1.0;

		at=acos(cst);
		da=at - Teq[atyp];
		df=da * Tk[atyp];
		e = df*da;
		e_theta = e_theta + e;
		df = df+df;
		at = sin(at);
		if (at > 0 && at < 1.e-3) at = 1.e-3;
		else if (at < 0 && at > -1.e-3) at = -1.e-3;
		df=-df/at;

		xtmp=df*rir*(dxjr-cst*dxir);
		dxtmp=df*rjr*(dxir-cst*dxjr);
		ytmp=df*rir*(dyjr-cst*dyir);
		dytmp=df*rjr*(dyir-cst*dyjr);
		ztmp=df*rir*(dzjr-cst*dzir);
		dztmp=df*rjr*(dzir-cst*dzjr);

		f[at1 + 0 ] += xtmp;
		f[at3 + 0 ] += dxtmp;
		f[at2 + 0 ] -= xtmp + dxtmp;
		f[at1 + 1 ] += ytmp;
		f[at3 + 1 ] += dytmp;
		f[at2 + 1 ] -= ytmp + dytmp;
		f[at1 + 2 ] += ztmp;
		f[at3 + 2 ] += dztmp;
		f[at2 + 2 ] -= ztmp + dztmp;

	}
    return( e_theta );
}

/* compute only for one residue */
_REAL ephiRes( nphi, a1, a2, a3, a4, atype, Pk, Pn, Phase, x, start, end )
int		nphi, *a1, *a2, *a3, *a4, *atype, start, end;
_REAL   *Pk, *Pn, *Phase, *x;
{
	_REAL e,co,den,co1,uu,vv,uv,ax,bx,cx,ay,by,cy,az,bz,cz;
	_REAL a0x,b0x,c0x,a0y,b0y,c0y,a0z,b0z,c0z,a1x,b1x;
	_REAL a1y,b1y,a1z,b1z,a2x,b2x,a2y,b2y,a2z,b2z;
	_REAL dd1x,dd2x,dd3x,dd4x,dd1y,dd2y,dd3y,dd4y,dd1z,dd2z,dd3z,dd4z;
	_REAL df,aa,bb,cc,ab,bc,ac,d1,d2,d3,e1,e2,e3,ktot;
	_REAL ktors1,ktors2,ktors3,ktors4,phase,e_tors;
	int i,at1,at2,at3,at4,atyp;
	int rat, iin, jin;

	e_tors = 0.0;
	for (i=0; i<nphi; i++) {

		at1 = a1[i]; at2 = a2[i]; at3 = abs(a3[i]); at4 = abs(a4[i]);
		rat = at1/3;
		iin = (rat<start || rat>=end) ? 0:1;
		rat = at4/3;
		jin = (rat<start || rat>=end) ? 0:1;
		if ( !iin && !jin )  continue;
		atyp = atype[i] - 1;

		ax = x[at2 + 0] - x[at1 + 0];
		ay = x[at2 + 1] - x[at1 + 1];
		az = x[at2 + 2] - x[at1 + 2];
		bx = x[at3 + 0] - x[at2 + 0];
		by = x[at3 + 1] - x[at2 + 1];
		bz = x[at3 + 2] - x[at2 + 2];
		cx = x[at4 + 0] - x[at3 + 0];
		cy = x[at4 + 1] - x[at3 + 1];
		cz = x[at4 + 2] - x[at3 + 2];

		ab = DOT(ax,ay,az,bx,by,bz);
		bc = DOT(bx,by,bz,cx,cy,cz);
		ac = DOT(ax,ay,az,cx,cy,cz);
		aa = DOT(ax,ay,az,ax,ay,az);
		bb = DOT(bx,by,bz,bx,by,bz);
		cc = DOT(cx,cy,cz,cx,cy,cz);

		uu = (aa * bb) - (ab*ab);
		vv = (bb * cc ) - (bc * bc);
		uv = (ab * bc) - (ac * bb);
			
		den= 1.0/sqrt(fabs(uu*vv));
		co = uv * den;
		co1 = 0.5*co*den;

		a0x = -bc*bx + bb*cx;
		a0y = -bc*by + bb*cy;
		a0z = -bc*bz + bb*cz;
	
		b0x = ab*cx + bc*ax -2.*ac*bx;
		b0y = ab*cy + bc*ay -2.*ac*by;
		b0z = ab*cz + bc*az -2.*ac*bz;
	
		c0x = ab*bx - bb*ax;
		c0y = ab*by - bb*ay;
		c0z = ab*bz - bb*az;
	
		a1x = 2.*uu*(-cc*bx + bc*cx);
		a1y = 2.*uu*(-cc*by + bc*cy);
		a1z = 2.*uu*(-cc*bz + bc*cz);
	
		b1x = 2.*uu*(bb*cx - bc*bx);
		b1y = 2.*uu*(bb*cy - bc*by);
		b1z = 2.*uu*(bb*cz - bc*bz);
	
		a2x = -2.*vv*(bb*ax - ab*bx);
		a2y = -2.*vv*(bb*ay - ab*by);
		a2z = -2.*vv*(bb*az - ab*bz);
	
		b2x = 2.*vv*(aa*bx - ab*ax);
		b2y = 2.*vv*(aa*by - ab*ay);
		b2z = 2.*vv*(aa*bz - ab*az);

		dd1x = (a0x - a2x*co1)*den;
		dd1y = (a0y - a2y*co1)*den;
		dd1z = (a0z - a2z*co1)*den;

		dd2x = (-a0x - b0x - (a1x - a2x - b2x)*co1)*den;
		dd2y = (-a0y - b0y - (a1y - a2y - b2y)*co1)*den;
		dd2z = (-a0z - b0z - (a1z - a2z - b2z)*co1)*den;

		dd3x = (b0x - c0x - (-a1x - b1x + b2x)*co1)*den;
		dd3y = (b0y - c0y - (-a1y - b1y + b2y)*co1)*den;
		dd3z = (b0z - c0z - (-a1z - b1z + b2z)*co1)*den;

		dd4x = (c0x - b1x*co1)*den;
		dd4y = (c0y - b1y*co1)*den;
		dd4z = (c0z - b1z*co1)*den;

multi_term:
		ktors1 = 0.; ktors2 = 0.; ktors3 = 0.; ktors4 = 0.;
		switch( (int)fabs(Pn[atyp]) ){  
			case 1: ktors1 = Pk[atyp]; break;
			case 2: ktors2 = Pk[atyp]; break;
			case 3: ktors3 = Pk[atyp]; break;
			case 4: ktors4 = Pk[atyp]; break;
			default:  fprintf( stderr, "bad value for Pn: %d %d %d %d %8.3f\n",
				at1,at2,at3,at4,Pn[atyp] ); exit(1);
		}
		if( fabs(Phase[atyp]-3.142) < 0.01  ) phase = -1.0;
			else phase = 1.0;

		e = 4.*ktors3*co*co + 2.*ktors2*co + ktors1 - 3.*ktors3
			+ 8.*ktors4*co*(co*co - 1.);
		e = e*co - ktors2 + ktors4;
		ktot = ktors1+ktors2+ktors3+ktors4;
		e = ktot + phase*e;
		e_tors += e;

	if( Pn[atyp] < 0 ){ atyp++; goto multi_term; }

	}
	return( e_tors );
}

_REAL ephi( nphi, a1, a2, a3, a4, atype, Pk, Pn, Phase, x, f )
int		nphi, *a1, *a2, *a3, *a4, *atype;
_REAL   *Pk, *Pn, *Phase, *x, *f;
{
	_REAL e,co,den,co1,uu,vv,uv,ax,bx,cx,ay,by,cy,az,bz,cz;
	_REAL a0x,b0x,c0x,a0y,b0y,c0y,a0z,b0z,c0z,a1x,b1x;
	_REAL a1y,b1y,a1z,b1z,a2x,b2x,a2y,b2y,a2z,b2z;
	_REAL dd1x,dd2x,dd3x,dd4x,dd1y,dd2y,dd3y,dd4y,dd1z,dd2z,dd3z,dd4z;
	_REAL df,aa,bb,cc,ab,bc,ac,d1,d2,d3,e1,e2,e3,ktot;
	_REAL ktors1,ktors2,ktors3,ktors4,phase,e_tors;
	int i,at1,at2,at3,at4,atyp;
	int rat, in1,in4;
/*	FILE *tmp=fopen("mmetor.list","w"); */

/*	etorrtmp = */ e_tors = 0.0;
	for (i=0; i<nphi; i++) {

		at1 = a1[i]; at2 = a2[i]; at3 = abs(a3[i]); at4 = abs(a4[i]);
		atyp = atype[i] - 1;
/*
		rat = at1/3;
		in1 = (rat>=debres && rat<endres) ? 1:0;
		rat = at4/3;
		in4 = (rat>=debres && rat<endres) ? 1:0;
*/
		ax = x[at2 + 0] - x[at1 + 0];
		ay = x[at2 + 1] - x[at1 + 1];
		az = x[at2 + 2] - x[at1 + 2];
		bx = x[at3 + 0] - x[at2 + 0];
		by = x[at3 + 1] - x[at2 + 1];
		bz = x[at3 + 2] - x[at2 + 2];
		cx = x[at4 + 0] - x[at3 + 0];
		cy = x[at4 + 1] - x[at3 + 1];
		cz = x[at4 + 2] - x[at3 + 2];

		ab = DOT(ax,ay,az,bx,by,bz);
		bc = DOT(bx,by,bz,cx,cy,cz);
		ac = DOT(ax,ay,az,cx,cy,cz);
		aa = DOT(ax,ay,az,ax,ay,az);
		bb = DOT(bx,by,bz,bx,by,bz);
		cc = DOT(cx,cy,cz,cx,cy,cz);

		uu = (aa * bb) - (ab*ab);
		vv = (bb * cc ) - (bc * bc);
		uv = (ab * bc) - (ac * bb);
			
		den= 1.0/sqrt(fabs(uu*vv));
		co = uv * den;
		co1 = 0.5*co*den;

		a0x = -bc*bx + bb*cx;
		a0y = -bc*by + bb*cy;
		a0z = -bc*bz + bb*cz;
	
		b0x = ab*cx + bc*ax -2.*ac*bx;
		b0y = ab*cy + bc*ay -2.*ac*by;
		b0z = ab*cz + bc*az -2.*ac*bz;
	
		c0x = ab*bx - bb*ax;
		c0y = ab*by - bb*ay;
		c0z = ab*bz - bb*az;
	
		a1x = 2.*uu*(-cc*bx + bc*cx);
		a1y = 2.*uu*(-cc*by + bc*cy);
		a1z = 2.*uu*(-cc*bz + bc*cz);
	
		b1x = 2.*uu*(bb*cx - bc*bx);
		b1y = 2.*uu*(bb*cy - bc*by);
		b1z = 2.*uu*(bb*cz - bc*bz);
	
		a2x = -2.*vv*(bb*ax - ab*bx);
		a2y = -2.*vv*(bb*ay - ab*by);
		a2z = -2.*vv*(bb*az - ab*bz);
	
		b2x = 2.*vv*(aa*bx - ab*ax);
		b2y = 2.*vv*(aa*by - ab*ay);
		b2z = 2.*vv*(aa*bz - ab*az);

		dd1x = (a0x - a2x*co1)*den;
		dd1y = (a0y - a2y*co1)*den;
		dd1z = (a0z - a2z*co1)*den;

		dd2x = (-a0x - b0x - (a1x - a2x - b2x)*co1)*den;
		dd2y = (-a0y - b0y - (a1y - a2y - b2y)*co1)*den;
		dd2z = (-a0z - b0z - (a1z - a2z - b2z)*co1)*den;

		dd3x = (b0x - c0x - (-a1x - b1x + b2x)*co1)*den;
		dd3y = (b0y - c0y - (-a1y - b1y + b2y)*co1)*den;
		dd3z = (b0z - c0z - (-a1z - b1z + b2z)*co1)*den;

		dd4x = (c0x - b1x*co1)*den;
		dd4y = (c0y - b1y*co1)*den;
		dd4z = (c0z - b1z*co1)*den;

multi_term:
		ktors1 = 0.; ktors2 = 0.; ktors3 = 0.; ktors4 = 0.;
		switch( (int)fabs(Pn[atyp]) ){  
			case 1: ktors1 = Pk[atyp]; break;
			case 2: ktors2 = Pk[atyp]; break;
			case 3: ktors3 = Pk[atyp]; break;
			case 4: ktors4 = Pk[atyp]; break;
			default:  fprintf( stderr, "bad value for Pn: %d %d %d %d %8.3f\n",
				at1,at2,at3,at4,Pn[atyp] ); exit(1);
		}
		if( fabs(Phase[atyp]-3.142) < 0.01  ) phase = -1.0;
			else phase = 1.0;

		e = 4.*ktors3*co*co + 2.*ktors2*co + ktors1 - 3.*ktors3
			+ 8.*ktors4*co*(co*co - 1.);
		e = e*co - ktors2 + ktors4;
		ktot = ktors1+ktors2+ktors3+ktors4;
		e = ktot + phase*e;

/*
		if ( in1 || in4 ) {
		  etorrtmp += e; 
		  fprintf("%4d %4d %4d %4d %f13.6\n",at1/3,at2/3,at3/3,at4/3,e); 
		}
*/
		e_tors += e;
		df = phase*( 12.*ktors3*co*co + 4.*ktors2*co + ktors1 - 3.*ktors3 
			+32.*ktors4*co*co*co - 16.*ktors4*co );

		f[at1 + 0] += df * dd1x;
		f[at1 + 1] += df * dd1y;
		f[at1 + 2] += df * dd1z;

		f[at2 + 0] += df * dd2x;
		f[at2 + 1] += df * dd2y;
		f[at2 + 2] += df * dd2z;

		f[at3 + 0] += df * dd3x;
		f[at3 + 1] += df * dd3y;
		f[at3 + 2] += df * dd3z;

		f[at4 + 0] += df * dd4x;
		f[at4 + 1] += df * dd4y;
		f[at4 + 2] += df * dd4z;

#ifdef PRINT_EPHI
		printf( "%4d %4d %4d %4d %4d %9.4f\n", i+1,at1/3,at2/3,at3/3,at4/3,e );
		printf( "%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f\n",
			-df * dd1x,-df * dd1y,-df * dd1z,-df * dd2x,-df * dd2y,-df * dd2z,
			-df * dd3x,-df * dd3y );
		printf( "%10.5f%10.5f%10.5f%10.5f\n",
			-df * dd3z,-df * dd4x,-df * dd4y,-df * dd4z );
#endif
	if( Pn[atyp] < 0 ){ atyp++; goto multi_term; }

	}
	return( e_tors );
}

/* MS June 25, 97 modified to reallocate the pair list if too small */
int	nblist( x, npairs, pairlist, prm, cut, maxnb, frozen )

_REAL *x, cut;
int  *npairs, **pairlist, *maxnb, *frozen;
parmstruct      *prm;


{
  _REAL dx,dy,dz,rrw,cut2, uppercut=(cut+22)*(cut+22);
  int  tot_pair,nx,ipair,ji,ires,jres,ip1,jp1,ip2,jp2,i,i3,j,j3;
  int  k,jrp,jrp2,kpr, nbdist=0, irescut, jrescut;
  int  *ires_pairlist;
  int  *iexw;
  int  *npairlist;

  ires_pairlist = ivector( 0, prm->Nres );
  iexw = ivector( -1, prm->Natom );
/*
    loop over all pairs of residues:
*/
  tot_pair = 0; kpr = 0; cut2 = cut*cut;
  ji = 0;
  for( i=0; i<prm->Natom; i++ ){
    iexw[i] = -1;
  }
  for( ires=0; ires<prm->Nres; ires++ ){
    ip1 = prm->Ipres[ires];
    ip2 = prm->Ipres[ires+1] - 1;
    jrp = 0; ires_pairlist[0] = ires;
/*      irescut = cut+resRad[ires]; */
    for( jres=ires+1; jres<prm->Nres; jres++ ){
      jp1 = prm->Ipres[jres];
      jp2 = prm->Ipres[jres+1] - 1;
/*        jrescut = resRad[jres]; */
      for( i=ip1-1; i<ip2; i++ ){
	i3 = 3*i;
	for( j=jp1-1; j<jp2; j++ ){
	  j3 = j*3;
	  dx = x[i3] - x[j3];
	  dy = x[i3 + 1] - x[j3 + 1];
	  dz = x[i3 + 2] - x[j3 + 2];
	  rrw = dx*dx + dy*dy + dz*dz;
	  nbdist++;
	  if( rrw < cut2 ) goto includit;
  	  /*if( rrw > (irescut+jrescut)*(irescut+jrescut)) break;*/
  	  if( rrw > uppercut) break;
	}
      }
      continue;
    includit:	
      ires_pairlist[++jrp] = jres;
    }
/*
     ---- End of generating the resdue pair list for residue ires 
     ---- Now take care of exclusions and sort the lists:
*/
    for( i=ip1-1; i<ip2; i++ ){
      ipair = 0;
      nx = prm->Iblo[i];
      for( k=0; k<nx; k++ ){
	iexw[ prm->ExclAt[ji+k]-1 ] = i;
      }
      ji += nx;
      for( jrp2=0; jrp2<=jrp; jrp2++ ){
	jres = ires_pairlist[jrp2];
	if( ires==jres ) jp1 = i+2; else jp1 = prm->Ipres[jres];
	jp2 = prm->Ipres[jres+1] - 1;
	for( j=jp1-1; j<jp2; j++ ){
	  if( iexw[j] != i && (!frozen[i] || !frozen[j]) ){ 
	    ipair++; (*pairlist)[kpr++] = j;
#ifdef REALLOCNB
	    if( kpr > (*maxnb) ) {
	      
	      printf( "reallocating space for %d non-bonded pairs\n", *maxnb + 100000 ); 
	      npairlist = ivector( 0, *maxnb + (*maxnb)/10 );
	      if ( !npairlist ) {
		fprintf( stderr, "failed to reallocate NB list \n");
		exit(-1);
	      }
	      memcpy( npairlist, (*pairlist), (*maxnb)*sizeof(int));
	      free_ivector( (*pairlist), 0, *maxnb );
	      (*pairlist) = npairlist;
	      *maxnb += (*maxnb)/10;
	    }
#endif
	  }
	}
      }
      tot_pair += ipair; npairs[i] = ipair;
#ifndef REALLOCNB
      if( tot_pair > (*maxnb) ){
	fprintf( stderr, "maxnb (%d) is too small needed %d\n", *maxnb, tot_pair );
	exit(1);
      }
#endif
    }
  }
  /*  printf( "nbdist:%d\n",nbdist ); */

#ifdef PRINT_NB_LIST
  printf( "npairs:\n" );
  for( k=0; k<prm->Natom; k++ ){
    printf( "%4d", npairs[k] );
    if( (k+1)%20 == 0 ) printf( "\n" );
  }
  printf( "\npairlist:\n" );
  for( k=0; k<tot_pair; k++ ){
    printf( "%4d", (*pairlist)[k] );
    if( (k+1)%20 == 0 ) printf( "\n" );
  }
  printf( "\n" );
#endif
  
  free_ivector( ires_pairlist, 0, prm->Nres );  
  free_ivector( iexw, -1, prm->Natom );
  /*
    printf( "                              " );
    printf( "                              " );
    printf( "        %d\n", tot_pair );
  */
  return( tot_pair );
}


/* MS December 01 modified to use BHtree */
int	nblistBH( x, npairs, pairlist, prm, cut, maxnb, frozen )

_REAL *x, cut;
int  *npairs, **pairlist, *maxnb, *frozen;
parmstruct      *prm;

{
  _REAL dx,dy,dz,rrw;
  int  tot_pair,nx,ipair,ji,ires,jres,ip1,jp1,ip2,jp2,i,j,k,jrp,jrp2,kpr;
  int  *ires_pairlist;
  int  *iexw;
  int  *npairlist;
  BHtree *bht;
  BHpoint **BHat;
  int res, close[2001], nbcl, *close_res, *atres;
  float pt[3];

  /* build a BHtree with all atoms */
  BHat = (BHpoint **)malloc(prm->Natom*sizeof(BHpoint *));

  res = 0;
  atres = ivector( 0, prm->Natom ); /* atomnum->resnum lookup table */
  for (i=0;i<prm->Natom;i++) {
    BHat[i] = (BHpoint *)malloc(sizeof(BHpoint));
    j = i*3;
    BHat[i]->x[0]=x[j];
    BHat[i]->x[1]=x[j+1];
    BHat[i]->x[2]=x[j+2];
    BHat[i]->r=0.0;
    BHat[i]->at=i;       /* atom number 0-based */
    if (i>=prm->Ipres[res+1]-1) res++;
    atres[i] = res; 
  }
  bht = generateBHtree(BHat,prm->Natom,10);

  ires_pairlist = ivector( 0, prm->Nres ); /* buffer list of residue pairs */
  close_res = ivector( 0, prm->Nres ); /* buffer for list of residue pair */
  iexw = ivector( -1, prm->Natom );
/*
    loop over all pairs of residues:
*/
  tot_pair = 0; kpr = 0;
  ji = 0;
  for( i=0; i<prm->Natom; i++ ){
    iexw[i] = -1;
  }

  /* for every residue */
  for( ires=0; ires<prm->Nres; ires++ ){
    ip1 = prm->Ipres[ires]; /* index of first atom in residue ires */
    ip2 = prm->Ipres[ires+1] - 1; /* index of last atom in residue ires */
    jrp = 0; ires_pairlist[0] = ires; 

    memset(close_res, 0, prm->Nres*sizeof(int));
    /* loop over all atoms in residue ires */
    for( i=ip1-1; i<ip2; i++ ){
      /* find all atoms within cutoff distance */
      j = i*3;
      pt[0] = x[j]; pt[1] = x[j+1]; pt[2] = x[j+2];
      nbcl = findBHcloseAtoms(bht, pt, cut, close, 2001);
      for (j=0; j<nbcl; j++) {
	res = atres[close[j]];
	if (res<=ires) continue;
	close_res[res] = 1;
      }
    }

    for (i=ires+1; i<prm->Nres; i++)
      if (close_res[i]) {
	ires_pairlist[++jrp] = i;
      }
/*
     ---- End of generating the resdue pair list for residue ires 
     ---- Now take care of exclusions and sort the lists:
*/
    for( i=ip1-1; i<ip2; i++ ){
      ipair = 0;
      nx = prm->Iblo[i];
      for( k=0; k<nx; k++ ){
	iexw[ prm->ExclAt[ji+k]-1 ] = i;
      }
      ji += nx;
      for( jrp2=0; jrp2<=jrp; jrp2++ ){
	jres = ires_pairlist[jrp2];
	if( ires==jres ) jp1 = i+2; else jp1 = prm->Ipres[jres];
	jp2 = prm->Ipres[jres+1] - 1;
	for( j=jp1-1; j<jp2; j++ ){
	  if( iexw[j] != i && (!frozen[i] || !frozen[j]) ){ 
	    ipair++; (*pairlist)[kpr++] = j;
#ifdef REALLOCNB
	    if( kpr > (*maxnb) ) {
	      
	      printf( "reallocating space for %d non-bonded pairs\n",
		      *maxnb + 100000 ); 
	      npairlist = ivector( 0, *maxnb + (*maxnb)/10 );
	      if ( !npairlist ) {
		fprintf( stderr, "failed to reallocate NB list \n");
		exit(-1);
	      }
	      memcpy( npairlist, (*pairlist), *maxnb*sizeof(int));
	      free_ivector( (*pairlist), 0, *maxnb );
	      (*pairlist) = npairlist;
	      *maxnb += (*maxnb)/10;
	    }
#endif
	  }
	}
      }
      tot_pair += ipair; npairs[i] = ipair;
#ifndef REALLOCNB
      if( tot_pair > (*maxnb) ){
	fprintf( stderr, "maxnb (%d) is too small\n", (*maxnb) );
	exit(1);
      }
#endif
    }
  }

#ifdef PRINT_NB_LIST
  printf( "npairs:\n" );
  for( k=0; k<prm->Natom; k++ ){
    printf( "%4d", npairs[k] );
    if( (k+1)%20 == 0 ) printf( "\n" );
  }
  printf( "\npairlist:\n" );
  for( k=0; k<tot_pair; k++ ){
    printf( "%4d", (*pairlist)[k] );
    if( (k+1)%20 == 0 ) printf( "\n" );
  }
  printf( "\n" );
#endif

  free_ivector( ires_pairlist, 0, prm->Nres );  
  free_ivector( iexw, -1, prm->Natom );
  free_ivector( close_res, 0, prm->Nres );
  free_ivector( atres, 0, prm->Natom );
  freeBHtree(bht);
/*
  printf( "                              " );
  printf( "                              " );
  printf( "        %d\n", tot_pair );
*/
  return( tot_pair );
}

int nbond( npairs, pairlist, x, f, enb, eel, ehb, enbfac, eelfac, prm, dield )

int		*npairs, *pairlist, dield;
_REAL	*x, *f;
_REAL	*enb,*eel,*ehb;
_REAL	enbfac, eelfac;
parmstruct      *prm;

{
	int		i,j,jn,ic,npr,lpair,iaci;
	_REAL	dumx,dumy,dumz,cgi,xw1,xw2,xw3,r2inv,df2,eelt,r6,r10,f1,f2;
	_REAL	df,fw1,fw2,fw3,enbfaci,eelfaci;
	_REAL	rinv, rs, rssq, eps1, epsi, cgijr, pow;
	int		ibig,isml;
/*	_REAL	hbener, nb14;  */

/*	nhbpair = 0; hbener = 0.0; nb14 = 0.0;  */
/*	enbrtmp = ehbrtmp = eelrtmp = 0.0; */
	nhbpair = 0;
#define SIG 0.3
#define DIW 78.0
#define C1 38.5

	lpair = 0; *enb = 0.; *eel = 0.; *ehb = 0.;
	enbfaci = 1./enbfac; eelfaci = 1./eelfac;
	for( i=0; i<prm->Natom-1; i++ ){
		npr = npairs[i];
		if( npr > 0 ){
			iaci = prm->Ntypes*(prm->Iac[i] - 1);
			dumx = 0.;
			dumy = 0.;
			dumz = 0.;
			cgi = eelfaci*prm->Charges[i];

			if( dield == -3 ){
                               /* special code RL dielectric, 94 force field */
				for( jn=0; jn<npr; jn++ ){
					j = pairlist[jn+lpair];
					xw1 = x[3*i + 0] - x[3*j + 0];
					xw2 = x[3*i + 1] - x[3*j + 1];
					xw3 = x[3*i + 2] - x[3*j + 2];
					r2inv = 1./(xw1*xw1 + xw2*xw2 + xw3*xw3);

									/* Ramstein & Lavery dielectric,
									   PNAS 85, 7231 (1988).         */
					rinv = sqrt( r2inv );
					rs = SIG/rinv;
					rssq = rs*rs;
					pow = exp( -rs );
					eps1 = rssq + rs + rs + 2.0;
					epsi = 1.0/(DIW - C1*pow*eps1);
					cgijr = cgi*prm->Charges[j]*rinv*epsi;
					*eel += cgijr;
					df2 = -cgijr*(1.0 + C1*pow*rs*rssq*epsi);

					ic = prm->Cno[iaci + prm->Iac[j] - 1] - 1;
					r6 = r2inv*r2inv*r2inv;
					f2 = prm->Cn2[ic]*r6;
					f1 = prm->Cn1[ic]*r6*r6;
/*
					if( f1 > 5000. )
						fprintf( stderr, "close contact: %d %d %8.2f\n", 
						i,j,f1 );
*/

					*enb += (f1-f2)*enbfaci;
					df = (df2+6.*(f2-f1-f1)*enbfaci)*r2inv;
					fw1 = xw1*df;
					fw2 = xw2*df;
					fw3 = xw3*df;
					dumx = dumx + fw1;
					dumy = dumy + fw2;
					dumz = dumz + fw3;
					f[3*j + 0] -= fw1;
					f[3*j + 1] -= fw2;
					f[3*j + 2] -= fw3;
				}
			} else if( dield == -4 ) {
                             /* distance-dependent dielectric code, 94 ff */
				for( jn=0; jn<npr; jn++ ){
					j = pairlist[jn+lpair];
					xw1 = x[3*i + 0] - x[3*j + 0];
					xw2 = x[3*i + 1] - x[3*j + 1];
					xw3 = x[3*i + 2] - x[3*j + 2];
					r2inv = 1./(xw1*xw1 + xw2*xw2 + xw3*xw3);

									/* epsilon = r  */
					rs = cgi*prm->Charges[j]*r2inv;
					df2 = -rs - rs;
					*eel += rs;
					ic = prm->Cno[iaci + prm->Iac[j] - 1] - 1;
					r6 = r2inv*r2inv*r2inv;
					f2 = prm->Cn2[ic]*r6;
					f1 = prm->Cn1[ic]*r6*r6;
					*enb += (f1-f2)*enbfaci;
					df = (df2+6.*(f2-f1-f1)*enbfaci)*r2inv;
					fw1 = xw1*df;
					fw2 = xw2*df;
					fw3 = xw3*df;
					dumx = dumx + fw1;
					dumy = dumy + fw2;
					dumz = dumz + fw3;
					f[3*j + 0] -= fw1;
					f[3*j + 1] -= fw2;
					f[3*j + 2] -= fw3;
				}
			} else {
                             /* standard code with all options   */
				for( jn=0; jn<npr; jn++ ){
					j = pairlist[jn+lpair];
					xw1 = x[3*i + 0] - x[3*j + 0];
					xw2 = x[3*i + 1] - x[3*j + 1];
					xw3 = x[3*i + 2] - x[3*j + 2];
					r2inv = 1./(xw1*xw1 + xw2*xw2 + xw3*xw3);

					/*  Code for various dielectric models.
						The df2 variable should hold r(dV/dr).    */

					if( dield == 0 ){
									/* epsilon = r  */
						rs = cgi*prm->Charges[j]*r2inv;
						df2 = -rs - rs;
						*eel += rs;
/*	if ( (i>=debres && i<endres) || (j>=debres && j < endres)) eelrtmp += rs; */

/*      if (tmp) {
	if ( (i>=debres && i<endres) || (j>=debres && j < endres))
	  fprintf(tmp, "%4d %4d %6.3f %13.6f\n",i,j,sqrt(xw1*xw1 + xw2*xw2 + xw3*xw3),rs);
      } */

					} else if( dield == 1 ){
									/* epsilon = 1  */
						rinv = sqrt( r2inv );
						df2 = -cgi*prm->Charges[j]*rinv;
                    	*eel -= df2;

					} else if( dield == -2 ){
									/* Ramstein & Lavery dielectric,
									   PNAS 85, 7231 (1988).         */
						rinv = sqrt( r2inv );
						rs = SIG/rinv;
						rssq = rs*rs;
						pow = exp( -rs );
						eps1 = rssq + rs + rs + 2.0;
						epsi = 1.0/(DIW - C1*pow*eps1);
						cgijr = cgi*prm->Charges[j]*rinv*epsi;
						*eel += cgijr;
						df2 = -cgijr*(1.0 + C1*pow*rs*rssq*epsi);
					}
					ic = prm->Cno[iaci + prm->Iac[j] - 1];
					if( ic > 0 || enbfac != 1.0 ){
						if( ic > 0 ) { 
							ic--;
						} else {
							ibig = prm->Iac[i] > prm->Iac[j] ? 
								prm->Iac[i] : prm->Iac[j];
							isml = prm->Iac[i] > prm->Iac[j] ? 
								prm->Iac[j] : prm->Iac[i];
							ic = ibig*(ibig-1)/2 + isml -1;
						}
						r6 = r2inv*r2inv*r2inv;
						f2 = prm->Cn2[ic]*r6;
						f1 = prm->Cn1[ic]*r6*r6;

/*						if( f1 > 5000. )
**						   fprintf( stderr, "close contact: %d %d %8.2f\n", 
**							i,j,f1 );
*/
						*enb += (f1-f2)*enbfaci;
/*	if ( (i>=debres && i<endres) || (j>=debres && j < endres)) enbrtmp += (f1-f2)*enbfaci; */
/*     if (tmp) {
       fprintf(tmp, "%4d %4d %6.3f %13.6f\n",i,j,sqrt(xw1*xw1 + xw2*xw2 + xw3*xw3),(f1-f2)*enbfaci);
     }*/
						df = (df2+6.*(f2-f1-f1)*enbfaci)*r2inv;
/*						if( enbfac != 1.0 ) nb14 += (f1-f2)*enbfaci;    */
					} else {
						ic = -ic - 1;
						r10 = r2inv*r2inv*r2inv*r2inv*r2inv;
						f2 = prm->HB10[ic]*r10;
						f1 = prm->HB12[ic]*r10*r2inv;
						*ehb += (f1-f2)*enbfaci;
/*	if ( (i>=debres && i<endres) || (j>=debres && j < endres)) ehbrtmp += (f1-f2)*enbfaci; */
						df = (df2+(10.*f2-12.*f1)*enbfaci)*r2inv;
						nhbpair++;
/*						 hbener += (f1-f2)*enbfaci;                     */
					}
					fw1 = xw1*df;
					fw2 = xw2*df;
					fw3 = xw3*df;
					dumx = dumx + fw1;
					dumy = dumy + fw2;
					dumz = dumz + fw3;
					f[3*j + 0] -= fw1;
					f[3*j + 1] -= fw2;
					f[3*j + 2] -= fw3;
				}
			}
			f[3*i + 0] += dumx;
			f[3*i + 1] += dumy;
			f[3*i + 2] += dumz;
			lpair += npr;
		}
	}
/*
	printf( "found %d hydrogen-bond pairs with energy %.3f\n", 
		nhbpair, hbener );
	printf( "1-4 nonbon = %.3f\n", nb14 );
*/
	return( 0 );
}

_REAL getTor( parmstruct *prm, _REAL *c, _REAL *f )
{
  _REAL eth, eta, dum;

  eth = ephi( prm->Nphih, prm->DihHAt1, prm->DihHAt2,
	     prm->DihHAt3, prm->DihHAt4, prm->DihHNum, 
	     prm->Pk, prm->Pn, prm->Phase, c, f);
  eta = ephi( prm->Mphia, prm->DihAt1, prm->DihAt2,
	     prm->DihAt3, prm->DihAt4, prm->DihNum,
	     prm->Pk, prm->Pn, prm->Phase, c, f);
  return(eth + eta);
}


void sanityCb(int cbNum, int nbat, _REAL *coords, _REAL *energies, int step)
{
  int i;

  printf("got there %d\n", step);
  for (i=0; i<10; i++)
    printf("x: %f y:%f z:%f\n", coords[i*3], coords[i*3+1], coords[i*3+2]);

  printf("\nenergies");
  for (i=0; i<10; i++)
    printf(" %f,", energies[i]);
  printf("\n===========================================\n");
  
}

cbFunc_t mme_callback[NCBFUNC];

void setccallback( sffcb_f cbfun, int frequency, int callbacknum)
{
  assert( callbacknum < NCBFUNC);
  mme_callback[callbacknum].fun = cbfun;
  mme_callback[callbacknum].freq = frequency;
}

_REAL mme( x, f, iter, ene, prm, opts)
_REAL   *x, *f, *ene;
int		*iter;
parmstruct      *prm;
SFFoptions      *opts;
{
	_REAL  ebh, eba, eth, eta, eph, epa, enb, ehb, eel, enb14, eel14, ecn, frms, dum;
	int  nb_pairs;
	int i,k;

	t1 = second();
	opts->enbr = opts->ehbr = opts->eelr = opts->etorr = opts->enb14r = opts->eel14r = 0.0;

	if( verbosemm && *iter<=1 ){ 
		printf( "    iter    bad        vdW     elect.     cons.     Total     grms\n" );
		tnonb = tpair = tbond = tangl = tphi = tcons = 0.0;
	}
	if( *iter==1 || (*iter%opts->nsnb==0 && *iter!=0 ) ) {

/*****************************************************************
Using BHtrees ends up being slower than n2 method on 4sgbEI

npairs with BHtree: 765186 in 0.401000
npairs: 765186 in 0.232000
ff:   1   2298.85  10082.88  -2463.31      0.00   9918.42    745.20 765186
ff:  10   2243.14    590.83  -2459.11      0.00    374.86     51.92 765186
ff:  20   1676.61   -777.55  -2603.81      0.00  -1704.74      4.00 765186
npairs with BHtree: 763404 in 0.375000
npairs: 763404 in 0.232000
ff:  30   1637.41   -957.61  -2747.24      0.00  -2067.44      2.91 763404
ff:  40   1621.13  -1044.20  -2839.55      0.00  -2262.63      2.32 763404
npairs with BHtree: 764524 in 0.378000
npairs: 764524 in 0.261000
ff:  50   1588.62  -1080.65  -2897.73      0.00  -2389.76      1.97 764524
ff:  60   1582.25  -1090.80  -2964.33      0.00  -2472.88      1.58 764524
ff:  70   1578.40  -1091.46  -3016.13      0.00  -2529.19      1.24 764524
npairs with BHtree: 762654 in 0.376000
npairs: 762654 in 0.232000



  		nb_pairs = nblistBH( x, opts->npairs, &(opts->pairlist), prm,
opts->cut, &(opts->maxnb), opts->frozen );
  		t2 = second();
  		printf( "npairs: %d in %f\n", nb_pairs, t2-t1);

***************************************************************************/

		t1 = second();
		nb_pairs = nblist( x, opts->npairs, &(opts->pairlist), prm,
				   opts->cut, &(opts->maxnb), opts->frozen);
		t2 = second();
		/*printf( "npairs: %d in %f\n", nb_pairs, t2-t1);  */
		tpair += t2-t1;  t1 = t2;

		/*
		printf( "npairs:\n" );
		for( k=0; k<prm->Natom; k++ ){
		  printf( "%4d |", npairs[k] );
		  if( (k+1)%20 == 0 ) printf( "\n" );
		}

		printf( "\npairlist: %p\n", pairlist );
		for( k=0; k<nb_pairs; k++ ){
		  printf( "%4d", pairlist[k] );
		  if( (k+1)%20 == 0 ) printf( "\n" );
		}
		printf( "\nDONE\n" );
		*/
	}

	for( i=0; i<3*prm->Natom; i++ ) f[i] = 0.0;

#ifdef WITH_CALLBACKS
	if (sffC_mme_callback_func[0] &&
	    (*iter%sffC_mme_callback_freq[0]==0 && *iter!=0) )
	  sffC_callback_before(*iter);
#endif

/*	tmp = fopen("mme.list","w"); */
	nbond( opts->npairs, opts->pairlist, x, f, &enb, &eel, &ehb, 1.0, 1.0, prm, opts->dield );
/*	fclose(tmp); */
	tmp = NULL;
/*
	enbr = enbrtmp;
	eelr = eelrtmp;
	ehbr = ehbrtmp;
*/
	ene[1] = enb;
	ene[2] = eel;
	ene[10] = ehb;
	t2 = second(); tnonb += t2-t1;  t1 = t2;

	ebh = ebond( prm->Nbonh, prm->BondHAt1, prm->BondHAt2,
		prm->BondHNum, prm->Rk, prm->Req, x, f );
	eba = ebond( prm->Mbona, prm->BondAt1, prm->BondAt2,
		prm->BondNum, prm->Rk, prm->Req, x, f );
	ene[3] = ebh + eba;
	t2 = second(); tbond += t2-t1;  t1 = t2;

	eth = eangl( prm->Ntheth, prm->AngleHAt1, prm->AngleHAt2,
		prm->AngleHAt3, prm->AngleHNum, prm->Tk, prm->Teq, x, f );
	eta = eangl( prm->Ntheta, prm->AngleAt1, prm->AngleAt2,
		prm->AngleAt3, prm->AngleNum, prm->Tk, prm->Teq, x, f );
	ene[4] = eth + eta;
	t2 = second(); tangl += t2-t1;  t1 = t2;

	eph = ephi( prm->Nphih, prm->DihHAt1, prm->DihHAt2,
		prm->DihHAt3, prm->DihHAt4, prm->DihHNum, 
		prm->Pk, prm->Pn, prm->Phase, x, f );
/*	etorr = etorrtmp; */
	epa = ephi( prm->Mphia, prm->DihAt1, prm->DihAt2,
		prm->DihAt3, prm->DihAt4, prm->DihNum,
		prm->Pk, prm->Pn, prm->Phase, x, f );
/*	etorr += etorrtmp; */

	ene[5] = eph + epa;

	ene[6] = 0.0;   /*  hbond226 term not in Amber-94 force field */
	dum= 0.0;
	nbond( prm->N14pairs, prm->N14pairlist, x, f, &enb14, &eel14, &ehb,
	      opts->scnb, opts->scee, prm, opts->dield );
/*
	enb14r = enbrtmp;
	eel14r = eelrtmp;
*/
	ene[7] = enb14 + ehb;
	ene[8] = eel14;
	t2 = second(); tphi += t2-t1;  t1 = t2;

	if( opts->nconstrained ){
		ecn = econs( x, f, prm, opts );
		t2 = second(); tcons += t2-t1;  t1 = t2;
	} else
		ecn = 0.0;
	ene[9] = ecn;

	ene[0] = 0.0;
	for( k=1; k<=10; k++ ) ene[0] += ene[k];

	for( k=0; k<prm->Natom; k++ ){    /* zero out frozen forces */
		if( opts->frozen[k] ){
			f[3*k + 0] = 0.0;
			f[3*k + 1] = 0.0;
			f[3*k + 2] = 0.0;
		}
	}

#ifdef PRINT_DERIV
	k=0;
	for(i=0; i<105; i++ ){
		k++; printf( "%10.5f", f[i] );
		if( k%8 == 0 ) printf( "\n" );
	}
	printf( "\n" );
#endif

	frms = 0.0;
	for( i=0; i<3*prm->Natom; i++) frms += f[i]*f[i];
	frms = sqrt( frms/(3*prm->Natom) );

	if (mme_callback[0].fun &&
	    ( *iter!=0 && ( (*iter) % mme_callback[0].freq)==0 ) )
	  mme_callback[0].fun(0, prm->Natom, x, ene, *iter);

	if( verbosemm && (*iter==1 || *iter%opts->ntpr==0) ){
	  if (verbosemm > 1) {
	    printf("1 nb    : %12.6f\n", ene[1]);
	    printf("2 eel   : %12.6f\n", ene[2]);
	    printf("3 bonds : %12.6f\n", ene[3]);
	    printf("4 angles: %12.6f\n", ene[4]);
	    printf("5 dihed.: %12.6f\n", ene[5]);
	    printf("6 NA    : %12.6f\n", ene[6]);
	    printf("7 14nb  : %12.6f\n", ene[7]);
	    printf("8 14eel : %12.6f\n", ene[8]);
	    printf("total   : %12.6f\n", ene[0]);
	  }
	  printf( "ff:%4d %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %d\n",
		 *iter, ene[3]+ene[4]+ene[5],ene[1]+ene[7]+ene[10],ene[2]+ene[8],ene[9],
		 ene[0],frms,nb_pairs  );
	  fflush( stdout );

	}

	return ( ene[0] );
}

_REAL ebond4( nbond, a1, a2, atype, Rk, Req, x, f )
int		nbond, *a1, *a2, *atype;
_REAL   *Rk, *Req, *x, *f;
{
	int i,at1,at2,atyp;
	_REAL  e_bond,r,rx,ry,rz,rw,r2,s,db,df,e;

	e_bond = 0.0;
	for( i=0; i<nbond; i++ ){
		at1 = 4*a1[i]/3; at2 = 4*a2[i]/3; atyp = atype[i] - 1;
		rx = x[at1    ] - x[at2    ];
		ry = x[at1 + 1] - x[at2 + 1];
		rz = x[at1 + 2] - x[at2 + 2];
		rw = x[at1 + 3] - x[at2 + 3];
		r2 = rx*rx + ry*ry + rz*rz + rw*rw;
		s = sqrt(r2);
		r = 2.0/s;
		db =  s - Req[atyp];
		df = Rk[atyp] * db;
		e = df*db;
		e_bond += e;
		df *= r;
		f[at1 + 0] += rx*df;
		f[at1 + 1] += ry*df;
		f[at1 + 2] += rz*df;
		f[at1 + 3] += rw*df;
		f[at2 + 0] -= rx*df;
		f[at2 + 1] -= ry*df;
		f[at2 + 2] -= rz*df;
		f[at2 + 3] -= rw*df;
	}
    return( e_bond );
}

_REAL eangl4( nang, a1, a2, a3, atype, Tk, Teq, x, f )
int		nang, *a1, *a2, *a3, *atype;
_REAL   *Tk, *Teq, *x, *f;
{
	int i,atyp,at1,at2,at3;
	_REAL  dxi,dyi,dzi,dwi,dxj,dyj,dzj,dwj,ri2,rj2,ri,rj,rir,rjr;
	_REAL  dxir,dyir,dzir,dwir,dxjr,dyjr,dzjr,dwjr,cst,at,da,df,e,e_theta;
	_REAL  xtmp,dxtmp,ytmp,wtmp,dytmp,ztmp,dztmp,dwtmp;

	e_theta = 0.0;
	for( i=0; i<nang; i++ ){
		at1 = 4*a1[i]/3; at2 = 4*a2[i]/3; at3 = 4*a3[i]/3; atyp = atype[i] - 1;

		dxi = x[at1    ] - x[at2    ];
		dyi = x[at1 + 1] - x[at2 + 1];
		dzi = x[at1 + 2] - x[at2 + 2];
		dwi = x[at1 + 3] - x[at2 + 3];
		dxj = x[at3    ] - x[at2    ];
		dyj = x[at3 + 1] - x[at2 + 1];
		dzj = x[at3 + 2] - x[at2 + 2];
		dwj = x[at3 + 3] - x[at2 + 3];

		ri2=dxi*dxi+dyi*dyi+dzi*dzi+dwi*dwi;
		rj2=dxj*dxj+dyj*dyj+dzj*dzj+dwj*dwj;
		ri=sqrt(ri2);
		rj=sqrt(rj2);
		rir=1./ri;
		rjr=1./rj;

		dxir=dxi*rir;
		dyir=dyi*rir;
		dzir=dzi*rir;
		dwir=dwi*rir;
		dxjr=dxj*rjr;
		dyjr=dyj*rjr;
		dzjr=dzj*rjr;
		dwjr=dwj*rjr;

		cst=dxir*dxjr+dyir*dyjr+dzir*dzjr+dwir*dwjr;
		if (cst > 1.0) cst = 1.0;
		if (cst < -1.0) cst = -1.0;

		at=acos(cst);
		da=at - Teq[atyp];
		df=da * Tk[atyp];
		e = df*da;
		e_theta = e_theta + e;
		df = df+df;
		at = sin(at);
		if (at > 0 && at < 1.e-3) at = 1.e-3;
		else if (at < 0 && at > -1.e-3) at = -1.e-3;
		df=-df/at;

		xtmp=df*rir*(dxjr-cst*dxir);
		dxtmp=df*rjr*(dxir-cst*dxjr);
		ytmp=df*rir*(dyjr-cst*dyir);
		dytmp=df*rjr*(dyir-cst*dyjr);
		ztmp=df*rir*(dzjr-cst*dzir);
		dztmp=df*rjr*(dzir-cst*dzjr);
		wtmp=df*rir*(dwjr-cst*dwir);
		dwtmp=df*rjr*(dwir-cst*dwjr);

		f[at1 + 0 ] += xtmp;
		f[at3 + 0 ] += dxtmp;
		f[at2 + 0 ] -= xtmp + dxtmp;
		f[at1 + 1 ] += ytmp;
		f[at3 + 1 ] += dytmp;
		f[at2 + 1 ] -= ytmp + dytmp;
		f[at1 + 2 ] += ztmp;
		f[at3 + 2 ] += dztmp;
		f[at2 + 2 ] -= ztmp + dztmp;
		f[at1 + 3 ] += wtmp;
		f[at3 + 3 ] += dwtmp;
		f[at2 + 3 ] -= wtmp + dwtmp;

	}
    return( e_theta );
}

_REAL ephi4( nphi, a1, a2, a3, a4, atype, Pk, Pn, Phase, x, f )
int		nphi, *a1, *a2, *a3, *a4, *atype;
_REAL   *Pk, *Pn, *Phase, *x, *f;
{
	_REAL e,co,den,co1,uu,vv,uv,ax,bx,cx,ay,by,cy,az,bz,cz,aw,bw,cw;
	_REAL a0x,b0x,c0x,a0y,b0y,c0y,a0z,b0z,c0z,a0w,b0w,c0w,a1x,b1x;
	_REAL a1y,b1y,a1z,b1z,a1w,b1w,a2x,b2x,a2y,b2y,a2z,b2z,a2w,b2w;
	_REAL dd1x,dd2x,dd3x,dd4x,dd1y,dd2y,dd3y,dd4y,dd1z,dd2z,dd3z,dd4z;
	_REAL dd1w,dd2w,dd3w,dd4w;
	_REAL df,aa,bb,cc,ab,bc,ac,d1,d2,d3,e1,e2,e3,ktot;
	_REAL ktors1,ktors2,ktors3,ktors4,phase,e_tors;
	int i,at1,at2,at3,at4,atyp;

	e_tors = 0.0;
	for (i=0; i<nphi; i++) {

		at1 = 4*a1[i]/3; at2 = 4*a2[i]/3; at3 = 4*abs(a3[i])/3; 
		at4 = 4*abs(a4[i])/3;
		atyp = atype[i] - 1;

		ax = x[at2 + 0] - x[at1 + 0];
		ay = x[at2 + 1] - x[at1 + 1];
		az = x[at2 + 2] - x[at1 + 2];
		aw = x[at2 + 3] - x[at1 + 3];
		bx = x[at3 + 0] - x[at2 + 0];
		by = x[at3 + 1] - x[at2 + 1];
		bz = x[at3 + 2] - x[at2 + 2];
		bw = x[at3 + 3] - x[at2 + 3];
		cx = x[at4 + 0] - x[at3 + 0];
		cy = x[at4 + 1] - x[at3 + 1];
		cz = x[at4 + 2] - x[at3 + 2];
		cw = x[at4 + 3] - x[at3 + 3];

#		define DOT4(a,b,c,d,e,f,g,h) a*e + b*f + c*g + d*h
		ab = DOT4(ax,ay,az,aw,bx,by,bz,bw);
		bc = DOT4(bx,by,bz,bw,cx,cy,cz,cw);
		ac = DOT4(ax,ay,az,aw,cx,cy,cz,cw);
		aa = DOT4(ax,ay,az,aw,ax,ay,az,aw);
		bb = DOT4(bx,by,bz,bw,bx,by,bz,bw);
		cc = DOT4(cx,cy,cz,cw,cx,cy,cz,cw);

		uu = (aa * bb) - (ab*ab);
		vv = (bb * cc ) - (bc * bc);
		uv = (ab * bc) - (ac * bb);
		den= 1.0/sqrt(uu*vv);
		co = uv * den;
		co1 = 0.5*co*den;

		a0x = -bc*bx + bb*cx;
		a0y = -bc*by + bb*cy;
		a0z = -bc*bz + bb*cz;
		a0w = -bc*bw + bb*cw;
	
		b0x = ab*cx + bc*ax -2.*ac*bx;
		b0y = ab*cy + bc*ay -2.*ac*by;
		b0z = ab*cz + bc*az -2.*ac*bz;
		b0w = ab*cw + bc*aw -2.*ac*bw;
	
		c0x = ab*bx - bb*ax;
		c0y = ab*by - bb*ay;
		c0z = ab*bz - bb*az;
		c0w = ab*bw - bb*aw;
	
		a1x = 2.*uu*(-cc*bx + bc*cx);
		a1y = 2.*uu*(-cc*by + bc*cy);
		a1z = 2.*uu*(-cc*bz + bc*cz);
		a1w = 2.*uu*(-cc*bw + bc*cw);
	
		b1x = 2.*uu*(bb*cx - bc*bx);
		b1y = 2.*uu*(bb*cy - bc*by);
		b1z = 2.*uu*(bb*cz - bc*bz);
		b1w = 2.*uu*(bb*cw - bc*bw);
	
		a2x = -2.*vv*(bb*ax - ab*bx);
		a2y = -2.*vv*(bb*ay - ab*by);
		a2z = -2.*vv*(bb*az - ab*bz);
		a2w = -2.*vv*(bb*aw - ab*bw);
	
		b2x = 2.*vv*(aa*bx - ab*ax);
		b2y = 2.*vv*(aa*by - ab*ay);
		b2z = 2.*vv*(aa*bz - ab*az);
		b2w = 2.*vv*(aa*bw - ab*aw);

		dd1x = (a0x - a2x*co1)*den;
		dd1y = (a0y - a2y*co1)*den;
		dd1z = (a0z - a2z*co1)*den;
		dd1w = (a0w - a2w*co1)*den;

		dd2x = (-a0x - b0x - (a1x - a2x - b2x)*co1)*den;
		dd2y = (-a0y - b0y - (a1y - a2y - b2y)*co1)*den;
		dd2z = (-a0z - b0z - (a1z - a2z - b2z)*co1)*den;
		dd2w = (-a0w - b0w - (a1w - a2w - b2w)*co1)*den;

		dd3x = (b0x - c0x - (-a1x - b1x + b2x)*co1)*den;
		dd3y = (b0y - c0y - (-a1y - b1y + b2y)*co1)*den;
		dd3z = (b0z - c0z - (-a1z - b1z + b2z)*co1)*den;
		dd3w = (b0w - c0w - (-a1w - b1w + b2w)*co1)*den;

		dd4x = (c0x - b1x*co1)*den;
		dd4y = (c0y - b1y*co1)*den;
		dd4z = (c0z - b1z*co1)*den;
		dd4w = (c0w - b1w*co1)*den;

multi_term:
		ktors1 = 0.; ktors2 = 0.; ktors3 = 0.; ktors4 = 0.;
		switch( (int)fabs(Pn[atyp]) ){
			case 1: ktors1 = Pk[atyp]; break;
			case 2: ktors2 = Pk[atyp]; break;
			case 3: ktors3 = Pk[atyp]; break;
			case 4: ktors4 = Pk[atyp]; break;
			default:  fprintf( stderr, "bad value for Pk: %d %d %d %d %8.3f\n",
				at1,at2,at3,at4,Pn[atyp] ); exit(1);
		}
		if( fabs(Phase[atyp]-3.142) < 0.01  ) phase = -1.0;
			else phase = 1.0;

		e = 4.*ktors3*co*co + 2.*ktors2*co + ktors1 - 3.*ktors3
			+ 8.*ktors4*co*(co*co - 1.);
		e = e*co - ktors2 + ktors4;
		ktot = ktors1+ktors2+ktors3+ktors4;
		e = ktot + phase*e;
		e_tors += e;

		df = phase*( 12.*ktors3*co*co + 4.*ktors2*co + ktors1 - 3.*ktors3 
			+32.*ktors4*co*co*co - 16.*ktors4*co );

		f[at1 + 0] += df * dd1x;
		f[at1 + 1] += df * dd1y;
		f[at1 + 2] += df * dd1z;
		f[at1 + 3] += df * dd1w;

		f[at2 + 0] += df * dd2x;
		f[at2 + 1] += df * dd2y;
		f[at2 + 2] += df * dd2z;
		f[at2 + 3] += df * dd2w;

		f[at3 + 0] += df * dd3x;
		f[at3 + 1] += df * dd3y;
		f[at3 + 2] += df * dd3z;
		f[at3 + 3] += df * dd3w;

		f[at4 + 0] += df * dd4x;
		f[at4 + 1] += df * dd4y;
		f[at4 + 2] += df * dd4z;
		f[at4 + 3] += df * dd4w;

#ifdef PRINT_EPHI
		printf( "%4d%4d%4d%4d%4d%8.3f\n", i+1,at1,at2,at3,at4,e );
		printf( "%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f\n",
			-df * dd1x,-df * dd1y,-df * dd1z,-df * dd2x,-df * dd2y,-df * dd2z,
			-df * dd3x,-df * dd3y );
		printf( "%10.5f%10.5f%10.5f%10.5f\n",
			-df * dd3z,-df * dd4x,-df * dd4y,-df * dd4z );
#endif

	if( Pn[atyp] < 0.0 ){ atyp++; goto multi_term; }
	}
	return( e_tors );
}

int	nblist4( x, npairs, pairlist, prm, cut, maxnb, frozen )

_REAL *x, cut;
int  *npairs, *pairlist, maxnb, *frozen;
parmstruct      *prm;

{
	_REAL dx,dy,dz,dw,rrw,cut2;
	int  tot_pair,nx,ipair,ji,ires,jres,ip1,jp1,ip2,jp2,i,j,k,jrp,jrp2,kpr;
	int  *ires_pairlist;
	int  *iexw;

	ires_pairlist = ivector( 0, prm->Nres );
	iexw = ivector( -1, prm->Natom );
/*
    loop over all pairs of residues:
*/
	tot_pair = 0; kpr = 0; cut2 = cut*cut;
	ji = 0;
	for( i=0; i<prm->Natom; i++ ){
		iexw[i] = -1;
	}
	for( ires=0; ires<prm->Nres; ires++ ){
		ip1 = prm->Ipres[ires];
		ip2 = prm->Ipres[ires+1] - 1;
		jrp = 0; ires_pairlist[0] = ires;
		for( jres=ires+1; jres<prm->Nres; jres++ ){
			jp1 = prm->Ipres[jres];
			jp2 = prm->Ipres[jres+1] - 1;
			for( i=ip1-1; i<ip2; i++ ){
				for( j=jp1-1; j<jp2; j++ ){
					dx = x[4*i + 0] - x[4*j + 0];
					dy = x[4*i + 1] - x[4*j + 1];
					dz = x[4*i + 2] - x[4*j + 2];
					dw = x[4*i + 3] - x[4*j + 3];
					rrw = dx*dx + dy*dy + dz*dz + dw*dw;
					if( rrw < cut2 ) goto includit;
				}
			}
			continue;
includit:	ires_pairlist[++jrp] = jres;
		}
/*
     ---- End of generating the resdue pair list for residue ires 
     ---- Now take care of exclusions and sort the lists:
*/
		for( i=ip1-1; i<ip2; i++ ){
			ipair = 0;
			nx = prm->Iblo[i];
			for( k=0; k<nx; k++ ){
				iexw[prm->ExclAt[ji+k]-1] = i;
			}
			ji += nx;
			for( jrp2=0; jrp2<=jrp; jrp2++ ){
				jres = ires_pairlist[jrp2];
				if( ires==jres ) jp1 = i+2; else jp1 = prm->Ipres[jres];
				jp2 = prm->Ipres[jres+1] - 1;
				for( j=jp1-1; j<jp2; j++ ){
					if( iexw[j] != i && (!frozen[i] || !frozen[j]) ){ 
						ipair++; pairlist[kpr++] = j;
					}
				}
			}
			tot_pair += ipair; npairs[i] = ipair;
			if( tot_pair > maxnb ){
				fprintf( stderr, "maxnb (%d) is too small (%d needed)\n", maxnb, tot_pair );
				exit(1);
			}
		}
	}

#ifdef PRINT_NB_LIST
	printf( "npairs:\n" );
	for( k=0; k<prm->Natom; k++ ){
		printf( "%4d", npairs[k] );
		if( (k+1)%20 == 0 ) printf( "\n" );
	}
	printf( "\npairlist:\n" );
	for( k=0; k<tot_pair; k++ ){
		printf( "%4d", pairlist[k] );
		if( (k+1)%20 == 0 ) printf( "\n" );
	}
	printf( "\n" );
#endif

	free_ivector( ires_pairlist, 0, prm->Nres );
	free_ivector( iexw, -1, prm->Natom );
 
	printf( "                              " );
	printf( "                              " );
	printf( "        %d\n", tot_pair );
	return( tot_pair );
}

int nbond4( npairs, pairlist, x, f, enb, eel, enbfac, eelfac, prm )

int		*npairs, *pairlist;
_REAL	*x, *f;
_REAL	*enb,*eel;
_REAL	enbfac, eelfac;
parmstruct      *prm;

{
	int		i,j,jn,ic,npr,lpair,iaci;
	_REAL	dumx,dumy,dumz,dumw,cgi,xw1,xw2,xw3,xw4,r2inv,df2,eelt,r6,f1,f2;
	_REAL	df,fw1,fw2,fw3,fw4,enbfaci,eelfaci,r10;
	int		ibig,isml;

	lpair = 0; *enb = 0.; *eel = 0.;
	enbfaci = 1./enbfac; eelfaci = 1./eelfac;
	for( i=0; i<prm->Natom-1; i++ ){

		npr = npairs[i];
		if( npr > 0 ){
			iaci = prm->Ntypes*(prm->Iac[i] - 1);
			dumx = 0.;
			dumy = 0.;
			dumz = 0.;
			dumw = 0.;
			cgi = -2.*prm->Charges[i];
			for( jn=0; jn<npr; jn++ ){
				j = pairlist[jn+lpair];
				xw1 = x[4*i + 0] - x[4*j + 0];
				xw2 = x[4*i + 1] - x[4*j + 1];
				xw3 = x[4*i + 2] - x[4*j + 2];
				xw4 = x[4*i + 3] - x[4*j + 3];
				r2inv = 1./(xw1*xw1 + xw2*xw2 + xw3*xw3 + xw4*xw4);
				df2 = eelfaci*cgi*prm->Charges[j]*r2inv;
				*eel -= df2;
				ic = prm->Cno[iaci + prm->Iac[j] - 1];
				if( ic > 0 || enbfac != 1.0 ){
					if( ic > 0 ) { 
						ic--;
					} else {
						ibig = prm->Iac[i] > prm->Iac[j] ? 
							prm->Iac[i] : prm->Iac[j];
						isml = prm->Iac[i] > prm->Iac[j] ? 
							prm->Iac[j] : prm->Iac[i];
						ic = ibig*(ibig-1)/2 + isml -1;
					}
					r6 = r2inv*r2inv*r2inv;
					f2 = prm->Cn2[ic]*r6;
					f1 = prm->Cn1[ic]*r6*r6;
					*enb += (f1-f2)*enbfaci;
					df = (df2+6.*(f2-f1-f1)*enbfaci)*r2inv;
				} else {
					ic = -ic - 1;
					r10 = r2inv*r2inv*r2inv*r2inv*r2inv;
					f2 = prm->HB10[ic]*r10;
					f1 = prm->HB12[ic]*r10*r2inv;
					*enb += (f1-f2)*enbfaci;
					df = (df2+(10.*f2-12.*f1)*enbfaci)*r2inv;
				}
				fw1 = xw1*df;
				fw2 = xw2*df;
				fw3 = xw3*df;
				fw4 = xw4*df;
				dumx += fw1;
				dumy += fw2;
				dumz += fw3;
				dumw += fw4;
				f[4*j + 0] -= fw1;
				f[4*j + 1] -= fw2;
				f[4*j + 2] -= fw3;
				f[4*j + 3] -= fw4;
			}
			f[4*i + 0] += dumx;
			f[4*i + 1] += dumy;
			f[4*i + 2] += dumz;
			f[4*i + 3] += dumw;
			lpair += npr;
		}
	}
	*eel *= 0.5;
	return( 0 );
}

_REAL mme4( x, f, iter, prm, opts )
_REAL   *x, *f;
int		*iter;
parmstruct      *prm;
SFFoptions      *opts;
{
	_REAL  ebh, eba, eth, eta, eph, epa, enb, eel, enb14, eel14, frms;
	_REAL	ene[20];
	int  nb_pairs;
	int i,k;
	_REAL	err;

	if( *iter<=1 ){ 
		printf( "    iter   bond    angle    dihed.     vdW    elect." );
		printf( "   e4d      Total     grms\n" );
	}
	if( *iter==1 || (*iter%opts->nsnb==0 && *iter!=0 ) ){
		nb_pairs = nblist4( x, opts->npairs, opts->pairlist, prm, opts->cut, opts->maxnb, opts->frozen );
/*		printf( "nb_pairs = %d\n", nb_pairs );          */
	}

	for( i=0; i<4*prm->Natom; i++ ) f[i] = 0.0;

	nbond4( opts->npairs, opts->pairlist, x, f, &enb, &eel, 1.0, 1.0, prm );
	ene[1] = enb;
	ene[2] = eel;

	ebh = ebond4( prm->Nbonh, prm->BondHAt1, prm->BondHAt2,
		prm->BondHNum, prm->Rk, prm->Req, x, f );
	eba = ebond4( prm->Mbona, prm->BondAt1, prm->BondAt2,
		prm->BondNum, prm->Rk, prm->Req, x, f );
	ene[3] = ebh + eba;

	eth = eangl4( prm->Ntheth, prm->AngleHAt1, prm->AngleHAt2,
		prm->AngleHAt3, prm->AngleHNum, prm->Tk, prm->Teq, x, f );
	eta = eangl4( prm->Ntheta, prm->AngleAt1, prm->AngleAt2,
		prm->AngleAt3, prm->AngleNum, prm->Tk, prm->Teq, x, f );
	ene[4] = eth + eta;

	eph = ephi4( prm->Nphih, prm->DihHAt1, prm->DihHAt2,
		prm->DihHAt3, prm->DihHAt4, prm->DihHNum, 
		prm->Pk, prm->Pn, prm->Phase, x, f );
	epa = ephi4( prm->Mphia, prm->DihAt1, prm->DihAt2,
		prm->DihAt3, prm->DihAt4, prm->DihNum,
		prm->Pk, prm->Pn, prm->Phase, x, f );
	ene[5] = eph + epa;

	ene[6] = 0.0;   /*  hbond term not in Amber-94 force field */

	nbond4( prm->N14pairs, prm->N14pairlist, x, f, &enb14, &eel14,
		opts->scnb, opts->scee, prm );
	ene[7] = enb14;
	ene[8] = eel14;

	ene[9] = 0.0;
	if ( opts->w4d != 0.0 ) {
		for( i = 0; i < prm->Natom; i++ ) {
			err = x[ 4*i + 3 ];
			f[ 4*i + 3 ] = opts->w4d*err;
			ene[9] += 0.5*opts->w4d*err*err;
		}
	}

	ene[0] = 0.0;
	for( k=1; k<=9; k++ ) ene[0] += ene[k];

	for( k=0; k<prm->Natom; k++ ){    /* zero out frozen forces */
		if( opts->frozen[k] ){
			f[4*k + 0] = 0.0;
			f[4*k + 1] = 0.0;
			f[4*k + 2] = 0.0;
			f[4*k + 3] = 0.0;
		}
	}

	frms = 0.0;
	for( i=0; i<4*prm->Natom; i++) frms += f[i]*f[i];
	frms = sqrt( frms/(4*prm->Natom) );

/*	for( i=0; i<4*prm->Natom; i+=4 )
 *		printf( "%10.5f%10.5f%10.5f%10.5f\n", f[i],f[i+1],f[i+2],f[i+3] );
*/

	if( verbosemm && (*iter==1 || *iter%opts->ntpr==0 )){
		printf( "ff:%4d%9.2f%9.2f%9.2f%9.2f%9.2f%9.2f%9.2f%9.2f\n",
			*iter, ene[3],ene[4],ene[5],ene[1]+ene[7],ene[2]+ene[8],ene[9],
			ene[0],frms );
	}

	return ( ene[0] );
}

int		get_masses( minv, prm , dim)
									/* dim = 3 or 4, for 3D or 4D problem */

_REAL		*minv;
parmstruct      *prm;
int             dim;

{

	int i,k;
	_REAL am;

	for( k=0, i=0; i<prm->Natom; i++ ){
		am = 1./prm->Masses[ i ];
		minv[ k + 0 ] = am;
		minv[ k + 1 ] = am;
		minv[ k + 2 ] = am;
		if( dim == 4 ) minv[ k + 3 ] = am;
		k += dim;
	}
	return( 0 );
}

int		md( n, maxstep, x, minv, f, v, mme, ene, prm, opts )

_REAL	( *mme )();
int		n, maxstep;
_REAL	*x, *minv, *f, *v, *ene;
parmstruct *prm;
SFFoptions *opts;

{

	_REAL  dtx, dt5, rndf, dttp, ekin0, ekin, epot, etot, tscal, temp, sd;
	int    nstep, i, k; 
	_REAL zero;

	assert(opts->temp0 >= 0.0);
	assert(opts->tempi >= 0.0);
	assert(opts->nfrozen >= 0);
	dtx = opts->dt*20.455;
	dt5 = 0.5*dtx;
	rndf = n - 6 -3*opts->nfrozen;
	ekin0  = opts->boltz2*rndf*opts->temp0;     /* target kinetic energy  */
	dttp = opts->dt/opts->tautp;
	zero = 0.0;
	assert(rndf >= 0.0);
	assert(ekin0 >= 0.0);

#ifdef USE_REAL_MASSES
	if (prm) get_masses( minv, prm, opts->dim);
		/* if no prmtop file is present, set all masses to 10 amu  */
	else for( i=0; i<n; i++ ) minv[ i ] = 0.1;
#else
	for( i=0; i<n; i++ ) minv[ i ] = 0.1;
#endif

	if( opts->zerov ){
		for( i=0; i<n; i++ ) v[ i ] = 0.0;
		ekin = 0.0;
	} else if( opts->tempi > 0.0 ){
		ekin = 0.0;
		for( i=0; i<n; i++ ){
			if( (prm) && (opts->frozen[ i/opts->dim ]) ){
				v[ i ] = 0.0;
			} else {	
				sd = sqrt(2.*opts->boltz2*opts->tempi*minv[i]);
				v[ i ] = gauss( &zero, &sd, &(opts->idum) );
				assert(minv[i] > 0.001);
				ekin += v[i]*v[i]/minv[i];
			}
		}
		ekin *= 0.5;
		temp = ekin/(opts->boltz2*rndf);
		assert(temp >= 0.0);

	} else {
		for( ekin=0., i=0; i<n; i++ )
			ekin += v[i]*v[i]*minv[i];
		ekin *= 0.5;
	}
	stop_flag = 0;
	sff_init_signals();


/*  main loop for the dynamics:    */

	for( nstep=1; nstep<=maxstep; nstep++ ){

		if(stop_flag) {	/* bsd */
			fprintf(stdout,"dynamics: STOP at iteration %d\n", nstep);
			break;
		}

		epot = mme( x, f, &nstep, ene, prm, opts );

		if( ekin > 0.01 ) tscal = sqrt(1. + dttp*(ekin0/ekin - 1.));
			else tscal = 1.0;
		ekin = 0.0;

		for( i=0; i<n; i++ ){
			v[ i ] = ( v[ i ] - f[ i ]*minv[ i ]*dtx ) * tscal;
			v[ i ] = v[i] > opts->vlimit ? opts->vlimit : v[i];
			v[ i ] = v[i] < -(opts->vlimit) ? -(opts->vlimit) : v[i];
			ekin += v[i]*v[i]/minv[i];

			x[ i ] += v[ i ]*dtx;
		}
		ekin *= 0.5;
		etot = ekin + epot;
		temp = ekin/(opts->boltz2*rndf);
		opts->t += opts->dt;
		assert(temp >= 0.0);
		assert(ekin >= 0.0);
		if(verbosemd && (nstep%opts->ntpr_md == 0 || nstep == 1 )){
			printf( "md:       %5d %10.3f %10.2f %10.2f %10.2f\n",
				nstep, opts->t, ekin, epot, temp );
			fflush( stdout );
		}
		if( opts->ntwx > 0 && nstep%opts->ntwx == 0 && opts->binposfp){
			writebinpos( n/3, x, opts->binposfp );
		}
	}
	sff_reset_signals();
	stop_flag = 0;
	return SFF_OK;
}


int		mme_timer()
{
	printf("\nTiming summary:\n" );
	printf("   bonds       %8.1f\n", tbond );
	printf("   angles      %8.1f\n", tangl );
	printf("   torsions    %8.1f\n", tphi  );
	printf("   pairlist    %8.1f\n", tpair );
	printf("   nonbonds    %8.1f\n", tnonb );
	printf("   constraints %8.1f\n", tcons );
	printf("   Total       %8.1f\n\n", tbond+tangl+tphi+tpair+tnonb+tcons );
	return( 0 );
}
