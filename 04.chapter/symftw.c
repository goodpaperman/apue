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
static int dopath (char const* path, Myfunc *); 

int main (int argc, char *argv[])
{
  int ret; 
  if (argc != 2)
    err_quit ("usage: ftw <starting-pathname>"); 

  ret = myftw (argv[1], myfunc); 
  exit (ret); 
}

#define FTW_F    1
#define FTW_D    2
#define FTW_DNR  3
#define FTW_NS   4

static int myftw (char *pathname, Myfunc *func)
{
  if (chdir (pathname) != 0)
    printf ("chdir %s failed\n", pathname); 

  return dopath (".", func); 
}

static int dopath (char const* path, Myfunc* func)
{
  struct stat statbuf; 
  struct dirent *dirp; 
  int ret; 
  DIR *dp; 
  char *ptr; 

  // may loop for dir soft links
  //if (stat (path, &statbuf) < 0)
  if (lstat (path, &statbuf) < 0)
    return func (path, &statbuf, FTW_NS); 

  char inbuf[PATH_MAX] = { 0 }; 
  char outbuf[PATH_MAX] = { 0 }; 
  char newpath[PATH_MAX] = { 0 }; 
  strcpy (newpath, path); 
  strcpy (inbuf, path); 
  while (S_ISLNK(statbuf.st_mode))
  {
    // handle symbolic to dir
    if (readlink (inbuf, outbuf, sizeof(outbuf)) < 0)
    {
        printf ("read symbolic path %s failed\n", inbuf); 
        return func (inbuf, &statbuf, FTW_NS); 
    }
    else 
    {
        if (lstat (outbuf, &statbuf) < 0)
            return func (outbuf, &statbuf, FTW_NS); 

        strcpy (newpath, outbuf); 
    }
  }

  if (S_ISDIR(statbuf.st_mode) == 0)
    return func (newpath, &statbuf, FTW_F); 

  if ((ret = func (newpath, &statbuf, FTW_D)) != 0)
    return ret; 

  ptr = newpath + strlen (newpath); 
  *ptr ++ = '/'; 
  *ptr = 0; 

  if ((dp = opendir (newpath)) == NULL)
  {
    ptr[-1] = 0; 
    return func (newpath, &statbuf, FTW_DNR); 
  }

  if (chdir (newpath) != 0)
    printf ("chdir %s failed\n", newpath); 

  while ((dirp = readdir (dp)) != NULL)
  {
    if (strcmp (dirp->d_name, ".") == 0 || 
        strcmp (dirp->d_name, "..") == 0)
        continue; 

    strcpy (ptr, dirp->d_name); 
    printf ("%s\n", newpath); 
    if ((ret = dopath (ptr, func)) != 0)
      break; 
  }

  ptr[-1] = 0; 
  if (chdir ("..") != 0)
    printf ("chdir back failed\n"); 

  if (closedir (dp) < 0)
    err_ret ("can't close directory %s", newpath); 

  return ret; 
}


static int myfunc (const char *pathname, const struct stat *statptr, int type)
{
  switch (type) 
  {
    case FTW_F:
      switch (statptr->st_mode & S_IFMT)
      {
        case S_IFREG: break; 
        case S_IFBLK: break; 
        case S_IFCHR: break; 
        case S_IFIFO: break; 
        case S_IFLNK: break; 
        case S_IFSOCK: break; 
        case S_IFDIR:
          err_dump ("for S_IFDIR for %s", pathname); 
      }
      break; 
    case FTW_D:
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

  return 0; 
}
