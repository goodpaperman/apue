#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <pthread.h> 
#include <errno.h>
//#include <dirent.h> 

static void* thread_start (void *arg)
{
  printf ("thread start %lu\n", pthread_self ()); 
  sleep (3); 
  printf ("thread exit %lu\n", pthread_self ()); 
  return 0; 
}

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc > 1)
  {
    // child mode
    pthread_t tid = atoll (argv[1]); 
    printf ("[%d] thread id: %s, tid = %lu\n", getpid (), argv[1], tid); 
    //ret = pthread_join (tid, NULL); 
    //if (ret != 0)
    //  err_sys ("pthread_join"); 
    //printf ("[%d] wait child %lu, ret = %d, err = %d", getpid (), tid, ret, errno); 
  }
  else 
  {
    pthread_attr_t attr; 
    ret = pthread_attr_init (&attr); 
    if (ret != 0)
      err_sys ("pthread_attr_init"); 

    pthread_t tid = 0; 
    ret = pthread_create (&tid, &attr, &thread_start, NULL); 
    if (ret != 0)
      err_sys ("pthread_create"); 

    pthread_attr_destroy (&attr); 
    pid_t pid = 0;
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      char tmp[32] = { 0 }; 
      sprintf (tmp, "%lu", tid); 
      execlp ("./exec_pthread", "./exec_pthread"/*argv[0]*/, tmp, NULL); 
      err_sys ("execlp error"); 
    }
    else 
    {
      printf ("fork and exec child %u\n", pid); 
      //sleep (6); 
      ret = pthread_join (tid, NULL); 
      if (ret != 0)
        err_sys ("pthread_join"); 

      printf ("wait child %lu\n", tid); 
    }
  }

  exit (0); 
}
