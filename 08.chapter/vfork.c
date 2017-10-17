#include "../apue.h"

int glob = 6; 
//char buf[] = "a write to stdout\n"; 

int main (void)
{
  int var = 88; 
  pid_t pid = 0;
  //if (write (STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof (buf)-1)
  //  err_sys ("write error"); 

  printf ("before fork\n"); 
  if ((pid = vfork ()) < 0)
    err_sys ("vfork error"); 
  else if (pid == 0)
  {
    glob ++; 
    var ++; 
#if 0
    _exit (0); 
#elif 0
    // flushing stdio !
    exit (0); 
#endif
  }
  // parent will not run util child exit !
  //else 
  //  sleep (2); 

  printf ("pid = %d, glob = %d, var = %d\n", getpid (), glob, var); 
  exit (0); 
}
