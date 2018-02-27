#include "../apue.h"
#include <sys/wait.h>
#include <errno.h> 
#include <signal.h> 
#include <linux/limits.h> 

#define MAX_BKGND 10
// SIGSTOP is non-maskable, use SIGUSR1 instead
#define SIG_STOP SIGUSR1
static void sighandler (int); 

#define JOB_FORE 1
#define JOB_BACK 2
#define JOB_STOP 3

struct jobinfo
{
    char cmd[PATH_MAX]; 
    pid_t pid; 
    int state; 
}; 

#define MAX_JOB 16
struct jobinfo jobs[MAX_JOB]; 

struct jobinfo* forejob ()
{
    for (int i=0; i<MAX_JOB; ++ i)
        if (jobs[i].state == JOB_FORE)
            return &jobs[i]; 

    return NULL; 
}

int addjob (char const* cmd, pid_t pid, int state)
{
    int n = 0; 
    for (; n<MAX_JOB; ++ n)
        if (jobs[n].pid == -1)
            break; 

    if (n == MAX_JOB)
        return 0; 

    jobs[n].pid = pid; 
    strcpy (jobs[n].cmd, cmd); 
    jobs[n].state = state; 
    return 1; 
}

void deletejob (pid_t pid)
{
    for (int i=0; i<MAX_JOB; ++ i)
    {
        if (jobs[i].pid == pid)
        {
            jobs[i].pid = -1; 
            jobs[i].state = 0; 
            jobs[i].cmd[0] = 0; 
        }
    }
}

void displayjob ()
{
    for (int i=0; i<MAX_JOB; ++ i)
        if (jobs[i].state != JOB_FORE && jobs[i].pid != -1)
            printf ("[%d] %d %s (%s)\n", i, jobs[i].pid, jobs[i].cmd, jobs[i].state == JOB_STOP ? "stop":"running"); 

    printf ("\n"); 
}

void initjob ()
{
    for (int i=0; i<MAX_JOB; ++ i)
        jobs[i].pid = -1; 
}

int do_builtin (char const* buf)
{
    int ret = 0; 
    if (strncmp (buf, "jobs", 4) == 0)
    {
      pid_t fore = tcgetpgrp (0); 
      printf ("curent task is %d\n", fore); 
      displayjob (); 
      return 1; 
    }
    else if (strncmp (buf, "fg", 2) == 0 || 
            strncmp (buf, "bg", 2) == 0)
    {
      int n = atoi (buf+3); 
      if (n >= MAX_JOB || jobs[n].pid == -1 
              || (buf[0] == 'b' && jobs[n].state != JOB_STOP)
              || (buf[0] == 'f' && jobs[n].state != JOB_FORE))
      {
        printf ("slot empty\n"); 
        return 2; 
      }

//#ifdef USE_KILL
#if 1
      ret = kill(-jobs[n].pid, SIGCONT); 
      if (ret != 0)
        printf ("kill CONT failed, errno %d\n", errno); 
      else 
      {
        printf ("kill CONT %d OK\n", jobs[n].pid); 
        jobs[n].state = buf[0] == 'f' ? JOB_FORE : JOB_BACK; 

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
      if (ret == 0 && buf[0] == 'f')
      {
        int status = 0; 
        pid_t pid = waitpid (jobs[n].pid, &status, 0); 
        if (pid < 0)
            printf ("waitpid error"); 
        else 
            printf ("wait %d %d\n", jobs[n].pid, status); 
      }

      return 1; 
    }

    return 0; 
}

int 
main (void)
{
  char buf [MAXLINE]; 
  pid_t pid; 
  int ret; 

  signal (SIGINT, sighandler); 
  //signal (SIGSTOP, sighandler);
  signal (SIGTSTP, sighandler); 
  signal (SIGQUIT, sighandler); 
  //signal (SIGHUP, sighandler); 
  //signal (SIGCHLD, sighandler); 
  
  struct sigaction act; 
  sigemptyset(&act.sa_mask); 
  act.sa_handler = sighandler; 
  // no SIGCHLD when child stopped.
  act.sa_flags = SA_NOCLDSTOP; 
  sigaction (SIGCHLD, &act, 0); 

  initjob (); 
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

    if (do_builtin (buf))
        // eat builtin command
        continue; 

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

    if (addjob (buf, pid, JOB_FORE))
    {
        int status = 0; 
        pid_t pid = waitpid (pid, &status, 0); 
        if (pid < 0)
            printf ("waitpid error"); 
        else 
            printf ("wait %d %d\n", pid, status); 
    }

    // parent
    printf ("%% "); 
#else 
    system (buf); 
#endif 
  } 

  exit (0); 
}

void 
sighandler (int signo)
{
  int ret = 0; 
  printf ("interrupt %d\n%% ", signo); 

  //signal (SIGSTOP, sighandler); 
  //signal (SIGCONT, sighandler); 
  if (signo == SIGHUP)
    exit (SIGHUP); 
  else if (signo == SIGTSTP)
  {
      struct jobinfo* job = forejob (); 
      if (job == NULL)
        printf ("no active foreground task running!\n"); 
      else 
      {
//#ifdef USE_KILL
#if 1
        ret = kill (-job->pid, SIGSTOP); 
        if (ret != 0)
          printf ("kill TSTP failed, errno %d\n", errno); 
        else 
        {
          printf ("kill TSTP %d OK\n", job->pid); 
          job->state = JOB_STOP; 
        }
#else 
        ret = tcsetpgrp (0, -1); 
        if (ret != 0)
          printf ("tcsetgrp failed, errno %d\n", errno); 
        else 
          printf ("tcsetgrp %d OK\n", job->pid); 
#endif
        
      }
  }
  else if (signo == SIGCHLD)
  {
    int status = 0; 
    pid_t pid = waitpid (-1, &status, 0); 
    if (pid < 0)
    {
      if (errno != ECHILD)
        printf ("waitpid error"); 
    }
    else 
    {
      printf ("wait child %d\n", pid); 
      deletejob (pid); 
    }
  }

  if (signo != SIGCHLD)
    signal (signo, sighandler); 
}
