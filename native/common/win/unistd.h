/* Windows stand-in for <unistd.h> (on the include path for MSVC builds only).
   The MGLTools sources include it but use little beyond the CRT basics. */
#ifndef MGL_WIN_UNISTD_H
#define MGL_WIN_UNISTD_H
#include <io.h>
#include <process.h>
#include <direct.h>
#include <stdlib.h>
#endif
