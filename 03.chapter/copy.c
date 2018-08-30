#include "../apue.h"

//#define BUFFSIZE 4096

void sig_xfsz (int signo)
{
    printf ("catch SIGXFSZ\n"); 
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
  while ((n = read (STDIN_FILENO, buf, bufsize)) > 0)
  {
    int m = write (STDOUT_FILENO, buf, n); 
    if (m != n)
      err_sys ("write error"); 
    else 
      fprintf (stderr, "write %d\n", m); 

#ifdef USE_DATASYNC
    fdatasync (STDOUT_FILENO); 
#endif 

#ifdef USE_SYNC
    fsync (STDOUT_FILENO); 
#endif

    if (n < 0)
      err_sys ("read error"); 
  }

  free(buf); 
  exit (0); 
}

