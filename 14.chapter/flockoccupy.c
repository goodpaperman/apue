#include "../apue.h" 
#include <errno.h> 
#include <fcntl.h> 
#include <sys/wait.h> 

int main (int argc, char *argv[])
{
  int fd; 
  pid_t pid; 
  char buf[5]; 
  struct stat statbuf; 
  if (argc != 3) { 
    fprintf (stderr, "usage: %s filename truncate\n", argv[0]); 
    exit (1); 
  }

  int trunc = atoi (argv[2]); 
  if (trunc)
  {
    fd = open (argv[1], O_RDWR | /*O_CREAT |*/ O_TRUNC, FILE_MODE); 
    if (fd < 0)
      err_sys ("open error"); 

    printf ("truncate OK\n"); 
    close (fd); 
  }
  else { 
    fd = open (argv[1], O_RDWR | O_CREAT, FILE_MODE); 
    if (fd < 0)
      err_sys ("open error"); 

    if (fstat (fd, &statbuf) < 0)
      err_sys ("fstat error"); 

    if (fchmod (fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
      err_sys ("fchmod error"); 

#if 0
    if (write_lock (fd, 0, SEEK_SET, 0) < 0)
      err_sys ("write_lock error"); 
#else
    if (read_lock (fd, 0, SEEK_SET, 0) < 0)
      err_sys ("read_lock error"); 
#endif

    printf ("occupy file %s\n", argv[1]); 
    sleep (60); 

    if (fchmod (fd, statbuf.st_mode) < 0)
      err_sys ("fchmod error"); 

    close (fd); 
  }

  return 0; 
}
