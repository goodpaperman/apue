#include "../apue.h"
#include <sys/wait.h>
#include <errno.h> 
#include <signal.h> 

#define MAX_BKGND 10
// SIGSTOP is non-maskable, use SIGUSR1 instead
#define SIG_STOP SIGUSR1
static void sig_int (int); 
pid_t g_bkgnd[MAX_BKGND] = { 0 }; 
pid_t g_foregnd = 0; 

int 
main (void)
{
  char buf [MAXLINE]; 
  pid_t pid; 
  int ret; 

  signal (SIGINT, sig_int); 
  signal (SIGQUIT, sig_int); 
  signal (SIGHUP, sig_int); 
  signal (SIGCHLD, sig_int); 
  //signal (SIGSTOP, sig_int);

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

    if (strncmp (buf, "ctrlz", 5) == 0)
    {
      pid_t now = g_foregnd; 
      if (now == 0)
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
#ifdef USE_KILL
        ret = kill (now, SIGSTOP); 
        if (ret != 0)
          printf ("kill STOP failed, errno %d\n", errno); 
        else 
          printf ("kill STOP %d OK\n", now); 
#else 
        ret = tcsetpgrp (0, -1); 
        if (ret != 0)
          printf ("tcsetgrp failed, errno %d\n", errno); 
        else 
          printf ("tcsetgrp %d OK\n", now); 
#endif
      }
      continue; 
    }
    else if (strncmp (buf, "jobs", 4) == 0)
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
        continue; 
      }

#ifdef USE_KILL
      ret = kill(g_bkgnd[index], SIGCONT); 
      if (ret != 0)
        printf ("kill CONT failed, errno %d\n", errno); 
      else 
      {
        printf ("kill CONT %d OK\n", g_bkgnd[index]); 
        g_bkgnd[index] = 0; 
      }
#else 
      ret = tcsetpgrp (0, g_bkgnd[index]); 
      if (ret != 0)
        printf ("tcsetpgrp failed, errno %d\n", errno); 
      else 
      {
        printf ("tcsetpgrp %d OK\n", g_bkgnd[index]); 
        g_bkgnd[index] = 0; 
      }
#endif 
      
      continue; 
    }

#if 1
    if ((pid = fork ()) < 0) {
      err_sys ("fork error"); 
    } else if (pid == 0) { 
      int n = 0; 
      char *args[10] = { 0 }; 
      char *ptr = buf; 
      // every task in seperate group
      ret = setpgid (0, 0); 
      if (ret != 0)
        err_ret ("setpgid (0, 0) failed"); 

      while ((args[n] = strtok(ptr, " ")) != NULL)
      {
        //printf ("split: %s\n", args[n]); 
        ptr = NULL; 
        n++; 
      }
      
      args[n] = NULL; 
      execvp (args[0], args); 
      err_ret ("couldn't execute: %s", buf); 
      exit (127); 
    }

    ret = setpgid (pid, 0); 
    if (ret != 0)
      err_ret ("setpgid (%d, 0) failed", pid); 

    // parent
    g_foregnd = pid; 
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
  //signal (SIGCHLD, sig_int); 
  struct sigaction act; 
  sigemptyset(&act.sa_mask); 
  act.sa_handler = sig_int; 
  // no SIGCHLD when child stopped.
  act.sa_flags = SA_NOCLDSTOP; 
  sigaction (SIGCHLD, &act, 0); 

  //signal (SIGSTOP, sig_int); 
  //signal (SIGCONT, sig_int); 
  if (signo == SIGHUP)
    exit (SIGHUP); 
  //else if (signo == SIGSTOP)
  else if (signo == SIGCHLD)
  {
    int status = 0; 
    pid_t pid = waitpid (-1, &status, 0); 
    if (pid < 0)
    {
      if (errno != ECHILD)
        err_sys ("waitpid error"); 
    }
    else 
      printf ("wait child %d\n", pid); 
  }

  //signal (SIGHUP, sig_int); 
}
