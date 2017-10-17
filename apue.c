#include "apue.h"
#include <errno.h> 
#include <limits.h> 
#include <stdarg.h> 
#include <sys/wait.h> 


static void err_doit (int, int, const char *, va_list); 

void 
err_dump (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, errno, fmt, ap); 
  va_end (ap); 
  abort (); 
  exit (1); 
}

void 
err_msg (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (0, 0, fmt, ap); 
  va_end (ap); 
}

void 
err_quit (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (0, 0, fmt, ap); 
  va_end (ap); 
  exit (1); 
}

void 
err_exit (int error, const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, error, fmt, ap); 
  va_end (ap); 
  exit (1); 
}

void 
err_ret (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, errno, fmt, ap); 
  va_end (ap); 
}

void 
err_sys (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, errno, fmt, ap); 
  va_end (ap); 
  exit (1); 
}

static void 
err_doit (int errnoflag, int error, const char *fmt, va_list ap)
{
  char buf [MAXLINE] = { 0 }; 
  vsnprintf (buf, MAXLINE, fmt, ap); 
  if (errnoflag)
    snprintf (buf+strlen (buf), MAXLINE-strlen (buf), ": %s", strerror (error)); 
  strcat (buf, "\n"); 
  fflush (stdout); 
  fputs (buf, stderr); 
  fflush (0); 
}


#ifdef PATH_MAX
static int pathmax = PATH_MAX; 
#else
static int pathmax = 0; 
#endif 

#define SUSV3 200112L

static long posix_version = 0; 

#define PATH_MAX_GUESS 1024

char* path_alloc (int *sizep)
{
  char *ptr; 
  int size; 
  if (posix_version == 0)
    posix_version = sysconf (_SC_VERSION); 

  if (pathmax == 0)
  {
    errno = 0; 
    if ((pathmax = pathconf ("/", _PC_PATH_MAX)) < 0) 
    {
      if (errno == 0)
        pathmax = PATH_MAX_GUESS; 
      else 
        err_sys ("pathconf error for _PC_PATH_MAX"); 
    }
    else 
      pathmax ++; 
  }

  if (posix_version < SUSV3) 
    size = pathmax + 1; 
  else 
    size = pathmax; 

  if ((ptr = malloc (size)) == NULL)
    err_sys ("malloc error for pathname"); 

  if (sizep != NULL)
    *sizep = size; 

  return ptr; 
}

#ifdef OPEN_MAX
static int openmax = OPEN_MAX; 
#else
static int openmax = 0; 
#endif 

#define OPEN_MAX_GUESS 256

long open_max (void)
{
  if (openmax == 0) { 
    errno = 0; 
    if ((openmax = sysconf (_SC_OPEN_MAX)) < 0) { 
      if (errno == 0) 
        openmax = OPEN_MAX_GUESS; 
      else 
        err_sys ("sysconf error for _SC_OPEN_MAX"); 
    }
  }

  return openmax; 
}

void set_fl (int fd, int flags)
{
  int val = fcntl (fd, F_GETFL, 0); 
  if (val < 0)
    err_sys ("fcntl F_GETFL error"); 

  val |= flags; 
  if (fcntl (fd, F_SETFL, val) < 0)
    err_sys ("fcntl F_SETFL error"); 
}


void clr_fl (int fd, int flags)
{
  int val = fcntl (fd, F_GETFL, 0); 
  if (val < 0)
    err_sys ("fcntl F_GETFL error"); 

  val &= ~flags; 
  if (fcntl (fd, F_SETFL, val) < 0)
    err_sys ("fcntl F_SETFL error"); 
}

void tell_buf (char const* name, FILE* fp)
{
  printf ("%s is: ", name); 
  if (fp->_flags & _IO_UNBUFFERED)
    printf ("unbuffered\n"); 
  else if (fp->_flags & _IO_LINE_BUF)
    printf ("line-buffered\n"); 
  else 
    printf ("fully-buffered\n"); 

  printf ("buffer size is %d\n", fp->_IO_buf_end - fp->_IO_buf_base); 
  printf ("discriptor is %d\n\n", fileno (fp)); 
}

void pr_exit (int status)
{
  if (WIFEXITED(status))
    printf ("normal termination, exit status = %d\n", WEXITSTATUS(status)); 
  else if (WIFSIGNALED(status))
    printf ("abnormal termination, signal number = %d%s\n", 
      WTERMSIG(status), 
#  ifdef WCOREDUMP
      WCOREDUMP(status) ? " (core file generated)" : ""); 
#  else 
      ""); 
#  endif 
  else if (WIFSTOPPED(status))
    printf ("child stopped, signal number = %d\n", WSTOPSIG(status)); 
}
