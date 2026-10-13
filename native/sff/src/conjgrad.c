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

#include <stdio.h>
#include "prm.h"
#include "memutil.h"

/* bsd mods */
/* extern int stop_flag; */
/* int sff_reset_signals(void); */
/* int sff_init_signals(void); */

int conjgrad( x, n, f, func, dgrad, dfpred, maxiter, prm, ene, opts ) 
_REAL	x[];
int		*n;
int		*maxiter;
_REAL	*f;
_REAL	( *func )();
_REAL	*dgrad;
_REAL	*dfpred;
parmstruct *prm;
_REAL   *ene;
SFFoptions * opts;
/*
**  ---routine carry out conjugate gradient optimization
**
**     x(n)   contains the variables to be updated
**     n      is the number of variables
**     f      will contain the value of the objective function at the end
**     func is the name of the function that computes the objective
**             function and its gradient
**     dgrad   return when sum of squares of gradient is less than dgrad
**     dfpred  expected decrease in the function on the first iteration
**
**     return codes:
**             >0    converged, final iteration number
**             -1    bad line search, probably an error in the relation
**                     of the funtion to its gradient (perhaps from
**                     round-off if you push too hard on the minimization).
**             -2    search direction was uphill
**             -3    exceeded the maximum number of iterations
**             -4    could not further reduce function value
**			   -5    stoped via signal	(bsd)
**
*/
{

	_REAL *w, *g;
    int maxlin = 5;
    int mxfcon = 2;
    
    int ret_val;
    _REAL r__1, r__2, r__3;
    
    _REAL calc, gama, beta, fmin, gmin, dfpr, drho, gnew;
    _REAL step, work, z, finit, ginit, gsqrd, gspln;
    _REAL stmin, gamden, ddspln, stepch, sbound;
    _REAL fch, rho, sum;
    int niter, iobs, ivar, i, nfbeg, iterc, irsdg, igopt;
    int nfopt, irsdx, ixopt, iginit;
    int iterfm, iterrs, iretry, ier;
	int ncopy;
	
	/* initialize signals */
	/* stop_flag = 0; */
/* 	sff_init_signals(); */	/* bsd */

    --x;

	ncopy = *n;
	w = Rvector( 1, 6*ncopy );
	g = Rvector( 1, ncopy );
    ier = 0;

    irsdx = *n;
    irsdg = irsdx + *n;
    iginit = irsdg + *n;
    ixopt = iginit + *n;
    igopt = ixopt + *n;

    iterc = 0;
    niter = 0;
    iterfm = iterc;

L10:
    ++niter;

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

/*
**   Compute the function value in "f" and its
**   gradient (with respect to x) in "g".
*/

    *f = ( *func )( &x[ 1 ], &g[ 1 ], &niter, ene, prm, opts );

/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    if (niter >= 2) {
		goto L30;
    }
L20:
    for (i = 1; i <= *n; ++i) w[i] = -g[i];
    iterrs = 0;
    if (iterc > 0) goto L40;

L30:
    gnew = (_REAL)0.;
    sum = (_REAL)0.;
    for (i = 1; i <= *n; ++i) {
		gnew += w[i] * g[i];
		r__1 = g[i];
		sum += r__1 * r__1;
    }
    if (niter == 1) {
		fmin = *f;
		gsqrd = sum;
		nfopt = niter;
		for (i = 1; i <= *n; ++i) {
	    	w[ixopt + i] = x[i];
	    	w[igopt + i] = g[i];
		}
		if (sum <= *dgrad) goto L100;
    } else {
		fch = *f - fmin;

		if (fch < (_REAL)0. || fch == (_REAL)0. && gnew / gmin >= (_REAL)-1.) {
	    	fmin = *f;
	    	gsqrd = sum;
	    	nfopt = niter;
	    	for (i = 1; i <= *n; ++i) {
				w[ixopt + i] = x[i];
				w[igopt + i] = g[i];
	    	}
	    	if (sum <= *dgrad) goto L100;
		}
    }
	/* if (stop_flag) { */	/* bsd */
/* 		fprintf(stdout,"conjgrad: STOP at iteration %d\n", niter); */
/* 		ier = -5; goto L100;  */
/* 	} */
    if (niter >= *maxiter) { ier = -3; goto L100; }
    if (niter > 1) goto L60;
    dfpr = *dfpred;
    stmin = *dfpred / gsqrd;

L40:
    ++iterc;

    finit = *f;
    ginit = (_REAL)0.;
    for (i = 1; i <= *n; ++i) {
		w[iginit + i] = g[i];
		ginit += w[i] * g[i];
    }
    if (ginit >= (_REAL)0.) { ier = -2; goto L90; }
    gmin = ginit;
    sbound = (_REAL)-1.;
    nfbeg = niter;
    iretry = -1;
    r__2 = stmin, r__3 = (r__1 = dfpr / ginit, ((r__1) >= 0 ? (r__1) : -(r__1)));
    stepch =  (r__2) <= (r__3) ? (r__2) : (r__3);
    stmin = (_REAL)0.;

L50:
    step = stmin + stepch;
    work = (_REAL)0.;
    for (i = 1; i <= *n; ++i) {
		x[i] = w[ixopt + i] + stepch * w[i];
		r__2 = work, r__3 = (r__1 = x[i] - w[ixopt + i], ((r__1) >= 0 ? (r__1) : -(r__1)));
		work = ((r__2) >= (r__3) ? (r__2) : (r__3));
    }
    if (work > (_REAL)0.) goto L10;
    if (niter > nfbeg + 1) ier = -1;
    if ((r__1 = gmin / ginit, ((r__1) >= 0 ? (r__1) : -(r__1))) > (_REAL).2)
		ier = -1;
    goto L90;

L60:
    work = (fch + fch) / stepch - gnew - gmin;
    ddspln = (gnew - gmin) / stepch;
    if (niter > nfopt) {
		sbound = step;
    }
    if (niter <= nfopt) {
		if (gmin * gnew <= (_REAL)0.) sbound = stmin;
		stmin = step;
		gmin = gnew;
		stepch = -stepch;
    }
    if (fch != (_REAL)0.) ddspln += (work + work) / stepch;

    if (gmin == (_REAL)0.) goto L90;
    if (niter > nfbeg + 1) {
		if ((r__1 = gmin / ginit, ((r__1) >= 0 ? (r__1) :
				 -(r__1))) <= (_REAL).2) {
	    	goto L90;
		}
		if (niter >= nfopt + maxlin) { ier = -1; goto L90; }
    }

L70:
    stepch = (sbound - stmin) * (_REAL).5;
    if (sbound < (_REAL)-.5) {
		stepch = stmin * (_REAL)9.;
    }
    gspln = gmin + stepch * ddspln;
    if (gmin * gspln < (_REAL)0.) stepch = stepch * gmin / (gmin - gspln);
    goto L50;

L80:
    sum = (_REAL)0.;
    for (i = 1; i <= *n; ++i) sum += g[i] * w[iginit + i];
    beta = (gsqrd - sum) / (gmin - ginit);

    if ((r__1 = beta * gmin, ((r__1) >= 0 ? (r__1) : -(r__1))) > gsqrd * (_REAL).2) {
		++iretry;
		if (iretry <= 0) {
	    	if (niter >= nfopt + maxlin) { ier = -1; goto L90; }
	    		else goto L70;
		}
    }

    if (*f < finit) iterfm = iterc;
    if (iterc >= iterfm + mxfcon) { ier = -4; goto L100; }
    dfpr = stmin * ginit;

    if (iretry > 0) {
		goto L20;
    }
    if (iterrs != 0 && iterc - iterrs < *n && (sum >= 0 ? sum : -sum) < gsqrd * (_REAL).2) {

		gama = (_REAL)0.;
		sum = (_REAL)0.;
		for (i = 1; i <= *n; ++i) {
	    	gama += g[i] * w[irsdg + i];
	    	sum += g[i] * w[irsdx + i];
		}
		gama /= gamden;
		if ((r__1 = beta * gmin + gama * sum, ((r__1) >= 0 ? 
			(r__1) : -(r__1))) < gsqrd * (_REAL).2) {
	    	for (i = 1; i <= *n; ++i)
				w[i] = -g[i] + beta * w[i] + gama * w[irsdx + i];
	    	goto L40;
		}
    }

    gamden = gmin - ginit;
    for (i = 1; i <= *n; ++i) {
		w[irsdx + i] = w[i];
		w[irsdg + i] = g[i] - w[iginit + i];
		w[i] = -g[i] + beta * w[i];
    }
    iterrs = iterc;
    goto L40;

L90:
    if (niter != nfopt) {
		*f = fmin;
		for (i = 1; i <= *n; ++i) {
	    	x[i] = w[ixopt + i];
	    	g[i] = w[igopt + i];
		}
    }
    if (ier == 0) goto L80;
L100:
/* 	stop_flag = 0; */
/* 	sff_reset_signals(); */
	free_Rvector( w, 1, 6*ncopy );  free_Rvector( g, 1, ncopy );
	if( ier == 0 ) ret_val = niter;
    	else ret_val = ier;
    return ret_val;
} 

