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
#include <stdlib.h>
#include "prm.h"

void	nrerror( msg )
char	msg[];
{

	fprintf( stderr, "FATAL: %s\n", msg );
	exit( 1 );
}

float	*vector( nl, nh )
int	nl, nh;
{
	float	*v;

	v = ( float * )malloc( ( nh - nl + 1 ) * sizeof( float ) );
	if( !v )
		nrerror( "allocation failure in vector()" );
	return( v - nl );
}

int	*ivector( nl, nh )
int	nl, nh;
{
	int	*v;

	v = ( int * )malloc( ( nh - nl + 1 ) * sizeof( int ) );
	if( !v )
		nrerror( "allocation failure in ivector()" );
	return( v - nl );
}

int	*ipvector( nl, nh )
int	nl, nh;
{
	int	*v;

	v = ( int * )malloc( ( nh - nl + 1 ) * sizeof( int * ) );
	if( !v )
		nrerror( "allocation failure in ivector()" );
	return( v - nl );
}

#define	NR_END	1
float	**matrix( nrl, nrh, ncl, nch )
int	nrl, nrh, ncl, nch;
{
	int	i;
	int	nrow, ncol;
	float	**m;

	nrow = nrh - nrl + 1;
	ncol = nch - ncl + 1;

	m = ( float ** )malloc( ( nrow + NR_END ) * sizeof( float * ) );
	if( !m )
		nrerror( "allocation failure 1 in matrix()" );
	m += NR_END;
	m -= nrl;

	m[ nrl ] = ( float * )
		malloc( ( nrow * ncol + NR_END ) * sizeof( float ) );
	if( !m[ nrl ] )
		nrerror( "allocation failure 2 in matrix()" );
	m[ nrl ] += NR_END;
	m[ nrl ] -= ncl;

	for( i = nrl + 1; i <= nrh; i++ )
		m[ i ] = m[ i - 1 ] + ncol;
	return( m );
}

int	**imatrix( nrl, nrh, ncl, nch )
int	nrl, nrh, ncl, nch;
{
	int	i;
	int	nrow, ncol;
	int	**m;

	nrow = nrh - nrl + 1;
	ncol = nch - ncl + 1;

	m = ( int ** )malloc( ( nrow + NR_END ) * sizeof( int * ) );
	if( !m )
		nrerror( "allocation failure 1 in matrix()" );
	m += NR_END;
	m -= nrl;

	m[ nrl ] = ( int * )
		malloc( ( nrow * ncol + NR_END ) * sizeof( int ) );
	if( !m[ nrl ] )
		nrerror( "allocation failure 2 in matrix()" );
	m[ nrl ] += NR_END;
	m[ nrl ] -= ncl;

	for( i = nrl + 1; i <= nrh; i++ )
		m[ i ] = m[ i - 1 ] + ncol;
	return( m );
}

void	free_vector( v, nl, nh )
float	*v;
int	nl, nh;
{

	free( ( char * )( v + nl ) );
}

void	free_ivector( v, nl, nh )
int	*v;
int	nl, nh;
{

	free( ( char * )( v + nl ) );
}

#ifdef ORIG
void	free_matrix( m, nrl, nrh, ncl, nch )
float	**m;
int	nrl, nrh, ncl, nch;
{
	free( ( char * )( m[ nrl ] + ncl - NR_END ) );
	free( ( char * )( m + nrl - NR_END ) );
}
#endif

void	free_imatrix( m, nrl, nrh, ncl, nch )
int	**m;
int	nrl, nrh, ncl, nch;
{
	free( ( char * )( m[ nrl ] + ncl - NR_END ) );
	free( ( char * )( m + nrl - NR_END ) );
}
/* MS */

_REAL	*Rvector( nl, nh )
int	nl, nh;
{
  _REAL	*v;

  v = ( _REAL * )malloc( ( nh - nl + 1 ) * sizeof( _REAL ) );
  if( !v )
    nrerror( "allocation failure in Rvector()" );
  return( v - nl );
}

void	free_Rvector( v, nl, nh )
_REAL	*v;
int	nl, nh;
{
  free( ( char * )( v + nl ) );
}
