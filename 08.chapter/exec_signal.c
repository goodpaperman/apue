#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
//#include <pthread.h> 
#include <errno.h>
//#include <dirent.h> 
#include <signal.h> 

void int_handler (int signo)
{
  printf ("caught %d\n", signo);
}

void int_handler2 (int signo)
{
  printf ("child caught %d\n", signo);
}

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc > 1)
  {
    // child mode
    int signo = atol (argv[1]); 
    printf ("[%d] argv[1] = %s, signo = %u\n", getpid (), argv[1], signo); 
#if 0
    sighandler_t h = signal (SIGINT, int_handler2); 
    printf ("install new SIGINT, previous handler %p\n", h); 
#elif 0 
    sighandler_t h = signal (SIGINT, SIG_IGN); 
    printf ("ignore SIGINT, previous handler %p\n", h); 
#else 
    sighandler_t h = signal (SIGCHLD, int_handler2); 
    printf ("install SIGCHLD, previous handler %p\n", h); 
#endif 
    sleep (3); 
  }
  else 
  {
#if 0
    sighandler_t h = signal (SIGINT, int_handler); 
    printf ("install SIGINT, previous handler %p\n", h); 
#elif 0 
    sighandler_t h = signal (SIGINT, SIG_IGN); 
    printf ("ignore SIGINT, previous handler %p\n", h); 
#else
    sighandler_t h = signal (SIGCHLD, int_handler); 
    printf ("install SIGCHLD, previous handler %p\n", h); 
#endif 

    pid_t pid = 0;
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      char tmp[32] = { 0 }; 
      sprintf (tmp, "%u", SIGINT); 
      execlp ("./exec_signal", "./exec_signal"/*argv[0]*/, tmp, NULL); 
      err_sys ("execlp error"); 
    }
    else 
    {
      printf ("fork and exec child %u\n", pid); 
      sleep (2); 
      kill (pid, SIGINT); 
      kill (getpid (), SIGINT); 
      sleep (3); 
    }
  }

  exit (0); 
}
