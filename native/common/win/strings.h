/* Windows stand-in for <strings.h> (MSVC builds only). */
#ifndef MGL_WIN_STRINGS_H
#define MGL_WIN_STRINGS_H
#include <string.h>
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
