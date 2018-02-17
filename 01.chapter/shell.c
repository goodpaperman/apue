#include "../apue.h"
#include <sys/wait.h>
#include <errno.h> 

#define MAX_BKGND 10
// SIGSTOP is non-maskable, use SIGUSR1 instead
#define SIG_STOP SIGUSR1
static void sig_int (int); 
pid_t g_bkgnd[MAX_BKGND] = { 0 }; 

int 
main (void)
{
  char buf [MAXLINE]; 
  pid_t pid; 
  int ret, status; 

  signal (SIGINT, sig_int); 
  signal (SIGQUIT, sig_int); 
  signal (SIGHUP, sig_int); 
  //signal (SIGSTOP, sig_int);
  signal (SIG_STOP, sig_int);

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

    //if (strncmp (buf, "ctrlz", 5) == 0)
    //{
    //  kill (getpid (), SIG_STOP); 
    //  continue; 
    //}
    if (strncmp (buf, "jobs", 4) == 0)
    {
      // show current foreground task
      for (int i=0; i<MAX_BKGND; ++ i)
      {
        if (g_bkgnd[i] != 0)
          printf ("%d: %d\n", i+1, g_bkgnd[i]); 
      }

      pid_t fore = tcgetpgrp (0); 
      printf ("curent task is %d\n", fore); 
      continue; 
    }
    else if (strncmp (buf, "fg", 2) == 0)
    {
      int index = -1; 
      for (int i=0; i<MAX_BKGND; ++ i)
      {
        if (g_bkgnd[i] != 0)
        {
          index = i; 
          break; 
        }
      }

      if (index == -1)
      {
        printf ("slot empty\n"); 
        return; 
      }

      ret = tcsetpgrp (0, g_bkgnd[index]); 
      if (ret != 0)
        printf ("tcsetgrp failed, errno %d\n", errno); 
      else 
      {
        printf ("tcsetgrp %d OK\n", g_bkgnd[index]); 
        g_bkgnd[index] = 0; 
      }
      
      //// make any background task foreground
      //int n = atoi (buf+3); 
      //ret = tcsetpgrp (0, g_bkgnd[n]); 
      //if (ret != 0)
      //  printf ("tcsetgrp failed, errno %d\n", errno); 
      //else 
      //  printf ("tcsetgrp %d OK\n",g_bkgnd[n]); 
      continue; 
    }

#if 0
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
#else 
    system (buf); 
#endif 
  } 

  exit (0); 
}

void 
sig_int (int signo)
{
  int ret = 0; 
  printf ("interrupt %d\n%% ", signo); 
  signal (SIGINT, sig_int); 
  signal (SIGQUIT, sig_int); 
  //signal (SIGSTOP, sig_int); 
  signal (SIG_STOP, sig_int); 
  //signal (SIGCONT, sig_int); 
  if (signo == SIGHUP)
    exit (SIGHUP); 
  //else if (signo == SIGSTOP)
  else if (signo == SIG_STOP)
  {
    pid_t now = tcgetpgrp (0); 
    if (now > 1)
      printf ("no active foreground task running!\n"); 
    else 
    {
      int index = -1; 
      for (int i=0; i<MAX_BKGND; ++ i)
      {
        if (g_bkgnd[i] == 0)
        {
          index = i; 
          break; 
        }
      }

      if (index == -1)
      {
        printf ("slot full\n"); 
        return; 
      }

      g_bkgnd[index] = now; 
      ret = tcsetpgrp (0, -1); 
      if (ret != 0)
        printf ("tcsetgrp failed, errno %d\n", errno); 
      else 
        printf ("tcsetgrp %d OK\n", -1); 
    }
  }

  //signal (SIGHUP, sig_int); 
}
