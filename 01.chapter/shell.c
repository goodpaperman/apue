#include "../apue.h"
#include <sys/wait.h>
#include <errno.h> 

static void sig_int (int); 

int 
main (void)
{
  char buf [MAXLINE]; 
  pid_t pid; 
  int status; 

  if (signal (SIGINT, sig_int) == SIG_ERR)
    err_sys ("signal error"); 

  printf ("%% "); 
  while (1) {
  //while (fgets (buf, MAXLINE, stdin) != 0) {
    errno = 0; 
    char *ptr = fgets (buf, MAXLINE, stdin); 
    if (ptr == 0)
    {
      if (errno == EINTR) {
        printf ("fgets terminal by signal, contining..\n%% "); 
        continue; 
      }
      else 
        break; 
    }

    if (buf [strlen (buf) - 1] == '\n') 
      buf [strlen (buf) - 1] = 0; 

    if ((pid = fork ()) < 0) {
      err_sys ("fork error"); 
    } else if (pid == 0) { 
      execlp (buf, buf, (char *) 0); 
      err_ret ("couldn't execute: %s", buf); 
      exit (127); 
    }

    // parent
    if ((pid = waitpid (pid, &status, 0)) < 0)
      err_sys ("waitpid error"); 

    printf ("%% "); 
  } 

  exit (0); 
}

void 
sig_int (int signo)
{
  printf ("interrupt\n%% "); 
  signal (SIGINT, sig_int); 
}
