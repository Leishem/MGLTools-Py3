
#include"invertMatrix.h"
#include<math.h>

void m4_submat( MATRIX4 mr, MATRIX3 mb, int i, int j )
{
	int ti, tj, idst, jdst;
	
	for ( ti = 0; ti < 4; ti++ )
	{
        if ( ti < i )
			idst = ti;
        else
			if ( ti > i )
				idst = ti-1;
			
			for ( tj = 0; tj < 4; tj++ )
			{
				if ( tj < j )
					jdst = tj;
				else
					if ( tj > j )
						jdst = tj-1;
					if ( ti != i && tj != j )
						mb[idst*3 + jdst] = mr[ti*4 + tj ];
			}
	}
}


VFLOAT m4_det( MATRIX4 mr )
{
	VFLOAT  det, result = 0, i = 1;
	MATRIX3 msub3;
	int     n;
	
	for ( n = 0; n < 4; n++, i *= -1 )
	{
        m4_submat( mr, msub3, 0, n );
		
        det     = m3_det( msub3 );
        result += mr[n] * det * i;
	}
	
	return( result );
}

int m4_inverse( MATRIX4 mr, MATRIX4 ma )
{
	VFLOAT  mdet = m4_det( ma );
	MATRIX3 mtemp;
	int     i, j, sign;
	
	if ( fabs( mdet ) == 0.0 )
        return( 0 );
	
	for ( i = 0; i < 4; i++ )
        for ( j = 0; j < 4; j++ )
		{
			sign = 1 - ( (i +j) % 2 ) * 2;
			
			m4_submat( ma, mtemp, i, j );
			
			mr[i+j*4] = ( m3_det( mtemp ) * sign ) / mdet;
		}
		
		return( 1 );
}

VFLOAT m3_det( MATRIX3 mat )
{
	VFLOAT det;
	
	det = mat[0] * ( mat[4]*mat[8] - mat[7]*mat[5] )
		- mat[1] * ( mat[3]*mat[8] - mat[6]*mat[5] )
		+ mat[2] * ( mat[3]*mat[7] - mat[6]*mat[4] );
	
	return( det );
}
