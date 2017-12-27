#include "../apue.h" 
#include <sys/wait.h> 
#include <limits.h> 

char *env_init[] = { "USER=unknown", "PATH=/tmp", NULL }; 

int main (int argc, char *argv[])
{
  pid_t pid; 
  char *exename = "echoall"; 
  //char *exename = "echoall.sh"; 
  //char *exename = "echoit.sh"; 
  //char *exename = "echoall.awk"; 
  if (argc > 1)
    exename = argv[1]; 

  if ((pid = fork ()) < 0)
    err_sys ("fork error"); 
  else if (pid == 0)
  {
    if (execlp (exename, exename, "myarg1", "MY ARG2", (char *)0, env_init) < 0)
      err_sys ("execlp error"); 
  }

  if (waitpid (pid, NULL, 0) < 0)
    err_sys ("wait error"); 

#if 0
  if ((pid = fork ()) < 0)
    err_sys ("fork error"); 
  else if (pid == 0)
  {
    if (execlp (exename, exename, "only 1 arg", (char *)0) < 0)
      err_sys ("execlp error"); 
  }
#endif 

  exit (0); 
}
