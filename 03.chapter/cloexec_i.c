#include "../apue.h"
#include <fcntl.h> 

int main (int argc, char *argv[])
{
  if (argc < 2)
  {
    printf ("Usage: cloexec_i 3\n"); 
    return -1; 
  }

  int ret = 0; 
  int fd = atoi (argv[1]); 
  printf ("get fd %d\n", fd); 
  char buf[512] = { 0 }; 
  lseek (fd, 0, SEEK_SET); 
  if ((ret = read (fd, buf, 512)) <= 0)
    err_sys ("read file"); 
  
  printf ("read %d: %s\n", fd, buf); 
  close (fd); 
  return 0; 
}
