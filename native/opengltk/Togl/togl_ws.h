#ifndef TOGL_WS_H
# define TOGL_WS_H

/* define windowing system togl is compiled with */
#ifdef _WIN32
# define TOGL_WGL
#else
# define TOGL_X11
#endif

#endif
