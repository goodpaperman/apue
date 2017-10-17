#include "../apue.h"
#include <fcntl.h> 

#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main (void)
{
  int fd = 0, om = 0; 
  om = umask (0); 
  printf ("mask before set to 0 is %o\n", om); 
  if ((fd = creat ("foo", RWRWRW) < 0))
    err_sys ("creat error for foo"); 

  close (fd); 
#if 1
  om = umask (S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); 
#else 
  om = umask (0777); 
#endif 
  printf ("mask before set to 0066 is %o\n", om); 
  if ((fd = creat ("bar", RWRWRW) < 0))
    err_sys ("creat error for bar"); 

  close (fd); 
  om = umask (0);
  printf ("last mask is %o\n", om); 
  exit (0); 
}
