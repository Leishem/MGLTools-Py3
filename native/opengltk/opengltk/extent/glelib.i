/*
 * copyright_notice
 */

%{
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
# include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/tube.h>
#include <pythonplus.h>
#include "opengltk.h"
%}

%include typemaps.i
%include gltypedef.i
%include gltypemap.i
%include glexception.i

typedef double gleDouble;

%include "gle_i.h"
