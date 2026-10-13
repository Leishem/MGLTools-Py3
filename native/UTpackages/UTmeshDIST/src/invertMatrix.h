typedef float MATRIX3[9];
typedef float MATRIX4[16];
typedef float VFLOAT;
void m4_submat( MATRIX4 mr, MATRIX3 mb, int i, int j );
VFLOAT m4_det( MATRIX4 mr );
int m4_inverse( MATRIX4 mr, MATRIX4 ma );
VFLOAT m3_det( MATRIX3 mat );
