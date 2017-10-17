#include "../apue.h"
#include <stdio.h> 
#include <errno.h> 
#include <signal.h> 
#include <fcntl.h> 

int sig_count = 0; 
void sigint_handler (int signum)
{
  printf ("signal %d recved %d\n", signum, sig_count ++); 
  signal (SIGINT, sigint_handler); 
}

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc < 2)
    err_sys ("Usage: read_inter file"); 

  signal (SIGINT, sigint_handler); 
  int fd = open (argv[1], O_RDONLY); 
  if (fd == -1)
    err_sys ("open failed"); 

  const int size = 16 * 1024 * 1024; 
  char *buf = (char *) malloc (size); 
  while ((ret = read (fd, buf, size)) > 0)
  {
    printf ("read %d\n", ret); 
    if (ret < size)
    {
      printf ("read less than require, errno = %d\n", errno); 
      break; 
    }
  }

  free(buf); 
  close (fd); 
  return 0; 
}
