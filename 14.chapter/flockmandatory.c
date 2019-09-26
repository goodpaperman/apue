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
  if (argc != 2) { 
    fprintf (stderr, "usage: %s filename\n", argv[0]); 
    exit (1); 
  }

  fd = open (argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
  if (fd < 0)
    err_sys ("open error"); 
  if (write (fd, "abcdef", 6) != 6)
    err_sys ("write error"); 

#if 1
  if (fstat (fd, &statbuf) < 0)
    err_sys ("fstat error"); 
  if (fchmod (fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
    err_sys ("fchmod error"); 
#endif

  SYNC_INIT (); 
  pid = fork (); 
  if (pid < 0) { 
    err_sys ("fork error"); 
  } else if (pid > 0) { 
    // parent
    if (write_lock (fd, 0, SEEK_SET, 0) < 0)
      err_sys ("write_lock error"); 

    SYNC_TELL (pid, 1); 
    if (waitpid (pid, NULL, 0) < 0)
      err_sys ("waitpid error"); 
  } else { 
    SYNC_WAIT (1); 
#if 1
    set_fl (fd, O_NONBLOCK); 
#endif
    if (read_lock (fd, 0, SEEK_SET, 0) != -1)
      err_sys ("child: read_lock succeeded!"); 

    printf ("read_lock of already-locked region returns %d\n", errno); 
    if (lseek (fd, 0, SEEK_SET) == -1)
      err_sys ("lseek error"); 
    if (read (fd, buf, 2) < 0)
      err_ret ("read failed (mandatory locking works)"); 
    else 
      printf ("read OK (no mandatory locking), buf = %2.2s\n", buf); 
  }

  return 0; 
}
