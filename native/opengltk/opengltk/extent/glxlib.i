/*
 * copyright_notice
 */
%module glxlib
%feature("python:cdefaultargs");
%{
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <pythonplus.h>

static int glx_DefaultScreen( Display* dpy)
{
  return DefaultScreen( dpy);
}
%}

%include typemaps.i
%include gltypedef.i
%include gltypemap.i
%include glexception.i

typedef int Bool;

%rename( DefaultScreen) glx_DefaultScreen;

%include "glx_i.h"

Display* XOpenDisplay( char const* display_name);
int XCloseDisplay( Display* display);

%constant GLXContext GLXContext_Null = NULL;
