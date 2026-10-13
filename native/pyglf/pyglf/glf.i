%module glf
%{
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifdef _MSC_VER
# include <windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else

#include <GL/gl.h>
#endif
#include "glf.h"


void glfGet3DSolidStringTriangles(char *s, void *returnVal)
{
  int i, nbt=0;
  
  for (i=0; i<(int)strlen(s); i++) {
    if (s[i]==' ') continue;
    nbt += glfCount3DSolidSymbolTriangles(s[i]);
  }

  //printf("found %d triangles\n ", nbt);

  vertexArray = (float *)malloc(nbt*3*3*sizeof(float));

  /* fixme test */
  normalArray = (float *)malloc(nbt*3*3*sizeof(float));
  tri =0;
  glfGet3DSolidString(s);
}
%}

%include "numarr.i"
%include "typemaps.i"

// typedefs form gl.h
typedef unsigned char	GLboolean;
typedef unsigned int	GLuint;		/* 4-byte unsigned */

void glfGet3DSolidStringTriangles(char *s, void *returnVal);

%apply float *OUTPUT {float *minx, float *miny, float *maxx, float *maxy };
%include "src/glf.h"
