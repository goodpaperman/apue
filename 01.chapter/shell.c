#include "../apue.h"
#include <sys/wait.h>
#include <errno.h> 

static void sighandler (int); 

int 
main (void)
{
  char buf [MAXLINE]; 

  signal (SIGINT, sighandler); 
  signal (SIGQUIT, sighandler); 
  signal (SIGHUP, sighandler); 

  //printf ("%s%% ", ttyname(0)); 
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

    while (strlen(buf) > 0 && buf [strlen (buf) - 1] == '\n') 
      buf [strlen (buf) - 1] = 0; 

#ifdef USE_FORK_EXEC
    pid_t pid; 
    int status; 
    if ((pid = fork ()) < 0) {
      err_sys ("fork error"); 
    } else if (pid == 0) { 
      execlp (buf, buf, (char *) 0); 
      err_ret ("couldn't execute: %s, errno %d", buf, errno); 
      exit (127); 
    }

    // parent
    if ((pid = waitpid (pid, &status, 0)) < 0)
      err_sys ("waitpid error"); 

#else 
      //printf ("system %s\n", buf); 
      system (buf); 
#endif 
    printf ("%% "); 
  } 

  exit (0); 
}

void 
sighandler (int signo)
{
  printf ("sig : %d\n", signo); 
  signal (SIGINT, sighandler); 
  signal (SIGQUIT, sighandler); 
}
