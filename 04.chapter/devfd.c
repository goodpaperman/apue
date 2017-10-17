#include "../apue.h"
#include <errno.h>
//#include <pwd.h>
//#include <dirent.h> 
#include <time.h> 
//#include <libgen.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <unistd.h> 


int main (int argc, char *argv[])
{
  char const* path = "/dev/fd/1"; 
  int ret = unlink (path); 
  if (ret == -1)
    printf ("unlink failed, errno = %d\n", errno); 
  else 
    printf ("unlink OK\n"); 

  int fd = creat (path, 0777); 
  if (fd < 0)
    printf ("creat failed, errno = %d\n", errno); 
  else 
  {
    printf ("creat OK\n"); 
    close (fd); 
  }

  return 0; 
}
