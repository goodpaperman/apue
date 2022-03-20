#include "../apue.h"
#ifdef SOLARIS
#include <sys/mkdev.h> 
#elif 1 //LINUX
#include <sys/sysmacros.h> 
#endif 

int main (int argc, char *argv[])
{
  int i; 
  struct stat buf; 
  for (i=1; i<argc; ++ i)
  {
    printf ("%s: ", argv[i]); 
#if 0
    if (lstat (argv[i], &buf) < 0)
    {
      err_ret ("lstat error"); 
      continue; 
    }
#else
    if (stat (argv[i], &buf) < 0)
    {
      err_ret ("stat error"); 
      continue; 
    }
#endif

    printf ("dev = %d/%d", major(buf.st_dev), minor(buf.st_dev)); 
    if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode))
    {
      printf (" (%s) rdev = %d/%d", 
        (S_ISCHR(buf.st_mode)) ? "character" : "block", 
        major(buf.st_rdev), minor(buf.st_rdev)); 
    }
    printf ("\n"); 
  }
  
  exit (0); 
}
