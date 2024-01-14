#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/wait.h> 
#include <fcntl.h> 
#include <pthread.h> 
#include <errno.h>
//#include <dirent.h> 

void do_fork(int n)
{
  pid_t pid = 0;
  if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
  else if (pid == 0)
  {
      printf ("[%u] child running, thread %lu\n", getpid(), pthread_self()); 
      sleep (3); 
  }
  else 
  {
      printf ("fork and exec child %u in thread %lu\n", pid, pthread_self()); 
      sleep (n); 
  }
}

static void* thread_start (void *arg)
{
  printf ("thread start %lu\n", pthread_self ()); 
#ifdef FORK_IN_THREAD
  do_fork (2); 
#else 
  sleep (2); 
#endif
  printf ("thread exit %lu\n", pthread_self ()); 
  return 0; 
}

int main (int argc, char *argv[])
{
    int ret = 0; 
    pthread_t tid = 0; 
    ret = pthread_create (&tid, NULL, &thread_start, NULL); 
    if (ret != 0)
        err_sys ("pthread_create"); 

#ifndef FORK_IN_THREAD
    do_fork (4); 
#else
    sleep (4); 
    printf ("main thread exit %lu\n", pthread_self()); 
#endif
    exit (0); 
}
