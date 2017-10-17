#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
//#include <pthread.h> 
#include <errno.h>
//#include <dirent.h> 
//#include <signal.h> 
//#include <sys/mman.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <semaphore.h> 

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc > 1)
  {
    // child mode
    sem_t *s = 0; 
    sscanf (argv[1], "%p", &s); 
    printf ("[%d] argv[1] = %s, sem = %p\n", getpid (), argv[1], s); 
    printf ("sem post\n"); 
    ret = sem_post (s); 
    if (ret == -1)
      err_sys ("sem_post"); 
    else 
      printf ("sem_post OK\n"); 
  }
  else 
  {
    sem_t *s = sem_open ("/this-is-a-test-sem", O_CREAT /*| O_EXCL*/); 
    if (s == SEM_FAILED)
      err_sys ("sem_open"); 

    pid_t pid = 0;
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      //ret = sem_post (s); 
      //if (ret == -1)
      //  err_sys ("sem_post"); 

      char tmp[32] = { 0 }; 
      sprintf (tmp, "%p", s); 
      execlp ("./exec_sem", "./exec_sem"/*argv[0]*/, tmp, NULL); 
      err_sys ("execlp error"); 
    }
    else 
    {
      printf ("fork and exec child %u\n", pid); 
      ret = sem_wait (s); 
      if (ret == -1)
        err_sys ("sem_wait"); 

      sem_close (s); 
    }
  }

  exit (0); 
}
