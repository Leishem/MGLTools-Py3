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
#include <math.h>

#define	IM1	2147483563
#define	IM2	2147483399
#define	AM	( 1.0 / IM1 )
#define	IMM1	( IM1 - 1 )
#define	IA1	40014
#define	IA2	40692
#define	IQ1	53668
#define	IQ2	52774
#define	IR1	12211
#define	IR2	3791
#define	NTAB	32
#define	NDIV	( 1 + IMM1 / NTAB )
#define	EPS	1.2e-7
#define	RNMX	( 1.0 - EPS )

static	int	idum2 = 1234566789;
static	long	iy = 0;
static	long	iv[ NTAB ];

static int igset = 0;
static float gdev1;

float	rand2( idum )
int	*idum;
{
	int		j, k;
	float	temp;

	if( *idum <= 0 ){
		if( -*idum < 1 )
			*idum = 1;
		else
			*idum = -*idum;
		idum2 = *idum;
		for( j = NTAB + 7; j >= 0; j-- ){
			k = *idum / IQ1;
			*idum = IA1 * ( *idum - k * IQ1 ) - k * IR1;
			if( *idum < 0 )
				*idum += IM1;
			if( j < NTAB )
				iv[ j ] = *idum;
		}
		iy = iv[ 0 ];
	}
	k = *idum / IQ1;
	*idum = IA1 * ( *idum - k * IQ1 ) - k * IR1;
	if( *idum < 0 )
		*idum += IM1;
	k = idum2 / IQ2;
	idum2 = IA2 * ( idum2 - k * IQ2 ) - k * IR2;
	if( idum2 < 0 )
		idum2 += IM2; 
	j = iy / NDIV;
	iy = iv[ j ] - idum2;
	iv[ j ] = *idum;
	if( iy < 1 )
		iy += IMM1;
	if( ( temp = AM * iy ) > RNMX )
		return( RNMX );
	else
		return( temp );
}

float gauss( mean, sd, idum)
float *mean,*sd;
int *idum;
{
	float fac,gdev2,rsq,s1,s2;
	if( igset ){
		igset = 0;
		return( *sd*gdev1 + *mean );
	} else {
repeat:		s1 = 2.*rand2(idum) - 1.;
			s2 = 2.*rand2(idum) - 1.;
		rsq = s1*s1 + s2*s2;
		if( rsq >= 1. || rsq == 0.0 ) goto repeat;
		fac = sqrt(-2.*log(rsq)/rsq);
		gdev1 = s1*fac;
		gdev2 = s2*fac;
		igset = 1;
		return( *sd*gdev2 + *mean );
	}
}
