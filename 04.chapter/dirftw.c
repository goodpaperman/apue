#include "../apue.h"
//#include <linux/limits.h> 
//#include <fcntl.h> 
//#include <pwd.h> 
#include <errno.h> 
#include <dirent.h> 
#include <limits.h> 
#include <sys/time.h> 


typedef int Myfunc (const char*, const struct stat*, int); 

static Myfunc myfunc; 
static int myftw (char*, Myfunc *); 
static int dopath (Myfunc *); 

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot; 

int main (int argc, char *argv[])
{
  int ret; 
  if (argc != 2)
    err_quit ("usage: ftw <starting-pathname>"); 

  struct timeval start, end; 
  gettimeofday (&start, 0); 
  ret = myftw (argv[1], myfunc); 
  gettimeofday (&end, 0); 
  printf ("diff = (%ld, %ld)\n", end.tv_sec - start.tv_sec, end.tv_usec - start.tv_usec); 

  ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock; 
  if (ntot == 0)
    ntot = 1; 

  printf ("regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot); 
  printf ("directories   = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot); 
  printf ("block special = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot); 
  printf ("char special  = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot); 
  printf ("FIFOs         = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot); 
  printf ("symbolic links= %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot); 
  printf ("sockets       = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot); 
  exit (ret); 
}

#define FTW_F    1
#define FTW_D    2
#define FTW_DNR  3
#define FTW_NS   4

#ifdef USE_RELATIVE_PATH
static char fullpath[PATH_MAX]; 
#else 
static char* fullpath; 
#endif

static int myftw (char *pathname, Myfunc *func)
{
#ifdef USE_RELATIVE_PATH
  if (chdir (pathname) == 0)
    printf ("\nchdir %s OK\n", pathname); 
  else 
    printf ("\nchdir %s failed\n", pathname); 

  strcpy (fullpath, "."); 
#else
  int len; 
  fullpath = path_alloc (&len); 
  strncpy (fullpath, pathname, len); 
  fullpath[len-1] = 0; 
#endif 
  return (dopath (func)); 
}

static int dopath (Myfunc* func)
{
  struct stat statbuf; 
  struct dirent *dirp; 
  int ret; 
  DIR *dp; 
  char *ptr; 

  // may loop for dir soft links
  //if (stat (fullpath, &statbuf) < 0)
  if (lstat (fullpath, &statbuf) < 0)
    return (func (fullpath, &statbuf, FTW_NS)); 
  if (S_ISDIR(statbuf.st_mode) == 0)
    return (func (fullpath, &statbuf, FTW_F)); 

  if ((ret = func (fullpath, &statbuf, FTW_D)) != 0)
    return (ret); 

  ptr = fullpath + strlen (fullpath); 
  *ptr ++ = '/'; 
  *ptr = 0; 

  if ((dp = opendir (fullpath)) == NULL)
  {
    ptr[-1] = 0; 
    return (func (fullpath, &statbuf, FTW_DNR)); 
  }

#ifdef USE_RELATIVE_PATH
  if (chdir (fullpath) == 0)
    printf ("\nchdir %s OK\n", fullpath); 
  else 
    printf ("\nchdir %s failed\n", fullpath); 
#endif 
  while ((dirp = readdir (dp)) != NULL)
  {
    if (strcmp (dirp->d_name, ".") == 0 || 
        strcmp (dirp->d_name, "..") == 0)
        continue; 

#ifdef USE_RELATIVE_PATH
    strcpy (fullpath, dirp->d_name); 
#else
    strcpy (ptr, dirp->d_name); 
#endif

#if 0
    int off = telldir (dp); 
    printf ("%ld: %s\n", off, dirp->d_name); 
#else
    printf ("%s\n", dirp->d_name); 
#endif 

    if ((ret = dopath (func)) != 0)
      break; 
  }

  ptr[-1] = 0; 
#ifdef USE_RELATIVE_PATH
  if (chdir ("..") == 0)
    printf ("chdir back OK\n\n"); 
  else 
    printf ("chdir back failed\n\n"); 
#endif 

  if (closedir (dp) < 0)
    err_ret ("can't close directory %s", fullpath); 

  return (ret); 
}


static int myfunc (const char *pathname, const struct stat *statptr, int type)
{
  switch (type) 
  {
    case FTW_F:
      switch (statptr->st_mode & S_IFMT)
      {
        case S_IFREG: nreg ++; break; 
        case S_IFBLK: nblk ++; break; 
        case S_IFCHR: nchr ++; break; 
        case S_IFIFO: nfifo++; break; 
        case S_IFLNK: nslink++; break; 
        case S_IFSOCK: nsock++; break; 
        case S_IFDIR:
          err_dump ("for S_IFDIR for %s", pathname); 
      }
      break; 
    case FTW_D:
      ndir ++; 
      break; 
    case FTW_DNR:
      err_ret ("can't read directory %s", pathname); 
      break; 
    case FTW_NS:
      err_ret ("stat error for %s", pathname); 
      break; 
    default:
      err_dump ("unknown type %d for pathname %s", type, pathname); 
      break; 
  }

  return (0); 
}
