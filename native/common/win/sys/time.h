/* Windows stand-in for <sys/time.h> (MSVC builds only). */
#ifndef MGL_WIN_SYS_TIME_H
#define MGL_WIN_SYS_TIME_H
#include <time.h>

#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval { long tv_sec; long tv_usec; };
#endif

struct timezone;

static __inline int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  struct timespec ts;
  (void)tz;
  if (!tv) return 0;
  timespec_get(&ts, TIME_UTC);
  tv->tv_sec = (long)ts.tv_sec;
  tv->tv_usec = (long)(ts.tv_nsec / 1000);
  return 0;
}
#endif
