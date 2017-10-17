#include "../apue.h" 
#include <errno.h>
#include <limits.h> 

static void pr_sysconf (char*, int); 
static void pr_pathconf (char *, char *, int); 

int 
main (int argc, char *argv[])
{
  if (argc != 2)
    err_quit ("usage: conf <dirname>"); 

#ifdef _POSIX_OPEN_MAX
  printf ("_POSIX_OPEN_MAX defined to be %d\n", _POSIX_OPEN_MAX); 
#else 
  printf ("no symbol for _POSIX_OPEN_MAX\n"); 
#endif 

#ifdef OPEN_MAX
  printf ("OPEN_MAX defined to be %d\n", OPEN_MAX); 
#else 
  printf ("no symbol for OPEN_MAX\n"); 
#endif 

#ifdef _SC_OPEN_MAX
  pr_sysconf ("sysconf (_SC_OPEN_MAX) = ", _SC_OPEN_MAX); 
#else 
  printf ("no symbol for _SC_OPEN_MAX\n"); 
#endif 

#ifdef _POSIX_NAME_MAX
  printf ("_POSIX_NAME_MAX defined to be %d\n", _POSIX_NAME_MAX); 
#else 
  printf ("no symbol for _POSIX_NAME_MAX\n"); 
#endif 

#ifdef NAME_MAX
  printf ("NAME_MAX defined to be %d\n", NAME_MAX); 
#else 
  printf ("no symbol for NAME_MAX\n"); 
#endif 

#ifdef _PC_NAME_MAX
  pr_pathconf ("pathconf (_PC_NAME_MAX) = ", argv[1], _PC_NAME_MAX); 
#else 
  printf ("no symbol for _PC_NAME_MAX\n"); 
#endif 

  exit (0); 
}

static void 
pr_sysconf (char *msg, int name) 
{
  long val; 
  fputs (msg, stdout); 
  errno = 0; 
  if ((val = sysconf (name)) < 0) { 
    if (errno != 0) { 
      if (errno == EINVAL)
        fputs ("(not supported)\n", stdout); 
      else 
        err_sys ("sysconf error"); 
    }
    else
      fputs ("(no limit)\n", stdout); 
  }
  else
    printf ("%ld\n", val); 
}


static void 
pr_pathconf (char *msg, char *path, int name) 
{
  long val; 
  fputs (msg, stdout); 
  errno = 0; 
  if ((val = pathconf (path, name)) < 0) { 
    if (errno != 0) { 
      if (errno == EINVAL)
        fputs ("(not supported)\n", stdout); 
      else 
        err_sys ("pathconf error, path = %s", path); 
    }
    else
      fputs ("(no limit)\n", stdout); 
  }
  else
    printf ("%ld\n", val); 
}
