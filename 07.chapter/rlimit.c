#include "../apue.h"
#include <sys/time.h>
#include <sys/resource.h> 

#if defined(BSD) || defined (MACOS)
#  define FMT "%10lld  "
#else
#  define FMT "%10ld  "
#endif

#define doit(name) pr_limits(#name, name)
static void pr_limits (char const*, int); 

int main (int argc, char *argv[])
{
#ifdef RLIMIT_AS
  doit(RLIMIT_AS); 
#endif 
#ifdef RLIMIT_CORE
  doit(RLIMIT_CORE); 
#endif
#ifdef RLIMIT_CPU
  doit(RLIMIT_CPU); 
#endif
#ifdef RLIMIT_DATA
  doit(RLIMIT_DATA); 
#endif
#ifdef RLIMIT_FSIZE
  doit(RLIMIT_FSIZE); 
#endif
#ifdef RLIMIT_LOCKS
  doit(RLIMIT_LOCKS); 
#endif 
#ifdef RLIMIT_MEMLOCK
  doit(RLIMIT_MEMLOCK); 
#endif
#ifdef RLIMIT_NOFILE
  doit(RLIMIT_NOFILE); 
#endif 
#ifdef RLIMIT_NPROC
  doit(RLIMIT_NPROC); 
#endif
#ifdef RLIMIT_RSS
  doit(RLIMIT_RSS); 
#endif
#ifdef RLIMIT_SBSIZE
  doit(RLIMIT_SBSIZE); 
#endif
#ifdef RLIMIT_STACK
  doit(RLIMIT_STACK); 
#endif
#ifdef RLIMIT_VMEM
  doit(RLIMIT_VMEM);
#endif
  return 0; 
}

static void pr_limits (char const* name, int res)
{
  struct rlimit lmt; 
  if (getrlimit (res, &lmt) == -1)
    err_sys ("getrlimit error for %s", name); 

  printf ("%-14s  ", name); 
  if (lmt.rlim_cur == RLIM_INFINITY)
    printf ("(infinite)  "); 
  else 
    printf (FMT, lmt.rlim_cur); 

  if (lmt.rlim_max == RLIM_INFINITY)
    printf ("(infinite)  "); 
  else 
    printf (FMT, lmt.rlim_max); 

  putchar ('\n'); 
}
