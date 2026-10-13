#include <stdio.h>
#include "prm.h"

int openbinpos( fp )
FILE	*fp;
{
	char	magic[ 10 ];

	if( fread( magic, 1, 4, fp ) != 4 ){
		fprintf( stderr, "Couldn't read magic number from BINPOS\n" );
		return( -1 );
	}

	magic[ 4 ] = '\0';
	if( strcmp( magic, "fxyz" ) != 0 ){
		fprintf( stderr, "bad magic number \"%s\"\n", magic );
		return( -1 );
	}
	return( 0 );
}

int readbinpos( n_atom, apos, fp )
int		n_atom;
_REAL	apos[];
FILE	*fp;
{
	int	count, *n_atomf, size = sizeof(_REAL);

	if( fread( n_atomf, sizeof( int ), 1, fp ) != 1 ) {
		return( 1 ); 
	}
	if( 0 == strncmp( n_atomf,"fxyz",4 ) ) { 
		fread( n_atomf, size, 1, fp );
	}
	if( ( count = fread( apos, size, 3 * n_atom, fp ) )
		!= 3 * n_atom ){
		fprintf( stderr, "Could only read %d of %d atoms requested\n",
			count / 3, n_atom );
		return( -1 );
	}
	return( 0 );
}

int startbinpos( fp )
FILE	*fp;
{
    /* write magic number */
    fwrite( "fxyz", 4, 1, fp );
	return( 0 );
}

int writebinpos( n_atom, apos, fp )
int		n_atom;
_REAL	apos[];
FILE	*fp;
{
        int size = sizeof(_REAL);

	if (fp == NULL) return 0;
	fwrite( &n_atom, sizeof( int ), 1, fp ) ;
	fwrite( apos, size, 3 * n_atom, fp );
	fflush( fp );
	return( 0 );
}

