#include "../apue.h"

//#define BUFFSIZE 4096
#define USE_STDFILE

void sig_xfsz (int signo)
{
    fprintf (stderr, "catch SIGXFSZ\n"); 
    exit (-1); 
}

int main (int argc, char *argv[])
{
  int n; 
  int bufsize = 4096; 
  if (argc > 1)
    bufsize = atoi (argv[1]); 

#ifdef USE_O_SYNC
  set_fl (STDOUT_FILENO, O_SYNC); 
#endif 

  signal (SIGXFSZ, sig_xfsz); 

  //printf ("bufsize = %d", bufsize); 
  char *buf = (char *)malloc (bufsize); 

#ifdef USE_STDFILE
  int fd_in = STDIN_FILENO; 
  int fd_out = STDOUT_FILENO; 
#else
  int fd_in = open ("copy", O_RDONLY, 0644); 
  if (fd_in == -1)
      err_sys ("open in file error"); 

  int fd_out = open ("copy2", O_WRONLY | O_CREAT, 0644); 
  if (fd_out == -1)
      err_sys ("open out file error"); 
#endif

  while ((n = read (fd_in, buf, bufsize)) > 0)
  {
    int m = write (fd_out, buf, n); 
    if (m == 0)
      err_sys ("write error, n = %d", n); 
    else 
      fprintf (stderr, "write %d\n", m); 

#ifdef USE_DATASYNC
    fdatasync (fd_out); 
#endif 

#ifdef USE_SYNC
    fsync (fd_out); 
#endif

    //if (n < 0)
    //  err_sys ("read error"); 
  }

  free(buf); 
  exit (0); 
}

