#include "../apue.h"


int main (void)
{
  pid_t pid = 0;
  printf ("before fork\n"); 
  if ((pid = fork ()) < 0)
    err_sys ("fork error"); 
  else if (pid == 0)
  {
    while (1)
      printf ("this is child %d\n", getpid ()); 
  }
  else 
  {
    while (1)
      printf ("this is parent %d\n", pid); 
  }

  return 0; 
}
