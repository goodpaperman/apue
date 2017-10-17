#include "../apue.h"

int g_total = 1; 

int main (void)
{
  pid_t pid = 0;
  printf ("before fork\n"); 
  while (1)
  {
    int total = g_total; 
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      total = ++ g_total; 
      printf ("[%d] child proc %d\n", total, getpid ()); 
    }
    else 
      printf ("[%d] parent proc %d\n", total, getpid ()); 
  }

  exit (0); 
}
