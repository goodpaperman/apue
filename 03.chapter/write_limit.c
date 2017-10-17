#include "../apue.h"
#include <stdio.h> 
#include <errno.h> 
#include <signal.h>
#include <fcntl.h> 

void file_limit_handler (int signo)
{
  printf ("catch %d\n", signo); 
}

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc < 2)
    err_sys ("Usage: write_limit file"); 

  signal (SIGXFSZ, file_limit_handler); 
  int fd = open (argv[1], O_WRONLY | O_CREAT, 0644); 
  if (fd == -1)
    err_sys ("open failed"); 

  int const size = 10; 
  char buf[size]; 
  strcpy (buf, "ABCDEFGHI"); 
  while (1)
  {
    ret = write (fd, buf, size); 
    if (ret == -1)
    {
      printf ("write return %d, errno = %d\n", ret, errno); 
      break; 
    }

    if (ret < size)
    {
      printf ("write %d less than require, errno = %d\n", ret, errno); 
      //break; 
    }
  }

  close (fd); 
  return 0; 
}
