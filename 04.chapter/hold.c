#include "../apue.h"
#include <fcntl.h> 

int main (void)
{
  if (open ("file.tmp", O_RDWR) < 0)
    err_sys ("open error"); 
  if (unlink ("file.tmp") < 0)
    err_sys ("unlink error"); 
  printf ("file unlinked\n"); 
  sleep (30); 
  printf ("done\n"); 
  exit (0); 
}
