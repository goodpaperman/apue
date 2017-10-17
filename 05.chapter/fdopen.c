#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  int pipefd[2] = { 0 }; 
  pid_t cpid = 0; 
  char buf[512] = { 0 }; 
  FILE *fp = 0; 
  int n = 0, a = 0, b = 0, c = 0; 

  if (pipe (pipefd) == -1)
    err_sys ("pipe failed"); 

  cpid = fork (); 
  if (cpid == -1)
    err_sys ("fork failed"); 

  if (cpid == 0)
  {
    // child process read, close write
    close (pipefd[1]);
    fp = fdopen (pipefd[0], "r"); 
    if (fp == 0)
      err_sys ("fdopen read in child failed"); 

    while (n++ < 100)
    {
      ret = fscanf (fp, "%d %d %d ", &a, &b, &c); 
      if (ret != 3)
        break; 

      printf ("child read %d %d %d\n", a, b, c); 
    }

    printf ("cihld read %d, break\n", ret); 
    fclose (fp); 
  }
  else 
  {
    // parent process write, close read
    close (pipefd[0]); 
    fp = fdopen (pipefd[1], "w"); 
    if (fp == 0)
      err_sys ("fdopen write in parent failed"); 

    while (n++ < 100)
    {
      ret = fprintf (fp, "%d %d %d ", a, b, c); 
      if (ret == 0)
        break; 

      printf ("parent write %d %d %d\n", a++, b++, c++); 
    }

    printf ("parent write %d, break\n", ret); 
    fclose (fp); 
  }

  return 0; 
}
