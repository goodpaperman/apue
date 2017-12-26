#include "../apue.h"

int glob = 6; 
//char buf[] = "a write to stdout\n"; 

int test ()
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
    //sleep (1); 
    // vfork ensure parent run after child call exit or execute 
    // !
    glob ++; 
    var ++; 
#if 0
    _exit (0); 
#else
    // flushing stdio !
    // fflush (NULL); 
    // fclose(stdout); 
    //
    // will crash if comment following line
    //exit (0); 
#endif
  }
  // parent will not run util child exit !
  //else 
  //  sleep (2); 

  int ret = printf ("pid = %d, glob = %d, var = %d\n", getpid (), glob, var); 
  return ret; 
}

int main (void)
{
  int magic=42; 
  int ret = test (); 
  printf ("test return %d, magic %d\n", ret, magic); 
  //return ret; 
  exit (ret); 
}
