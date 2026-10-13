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
#include <GL/glsmap.h>
#include <pythonplus.h>
#include "callback.h"
#include "opengltk.h"
%}

%include typemaps.i
%include gltypedef.i
%include gltypemap.i
%include glexception.i

void smapSetPositionLightsFunc( SphereMap* smap,
				void_int_voidstar_f positionLights);
void smapSetDrawViewFunc( SphereMap* smap,
			  void_int_voidstar_f drawView);
void smapGetPositionLightsFunc( SphereMap* smap,
				void_int_voidstar_f* positionLights);
void smapGetDrawViewFunc( SphereMap* smap, void_int_voidstar_f* drawView);

%include "smap_i.h"
