#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
//#include <dirent.h> 

void bye (void)
{
  printf ("[%d] that was all, folks~\n", getpid ()); 
}

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc > 1)
  {
    // child mode
    printf ("[%d] atexit function: 0x%x, %s\n", getpid (), (unsigned)bye, argv[1]); 
  }
  else 
  {
    ret = atexit (bye); 
    if (ret != 0)
      perror ("atexit failed"); 

    pid_t pid = 0;
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      char tmp[32] = { 0 }; 
      sprintf (tmp, "%p", bye); 
      printf ("[%d] atexit function: %s\n", getpid (), tmp); 

      execlp ("./exec_atexit", "./exec_atexit"/*argv[0]*/, tmp, NULL); 
      err_sys ("execlp error"); 
    }
    else 
    {
      printf ("fork and exec child %u\n", pid); 
      sleep (1); 
    }
  }

  exit (0); 
}
