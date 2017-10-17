#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 

//#ifndef O_CLOEXEC
//#  define O_CLOEXEC 02000000
//#endif 

int main (int argc, char *argv[])
{
  int fd = 0; 
  //printf ("argc %d, argv[0] %s\n", argc, argv[0]); 
  if (argc > 1)
  {
    // child mode
    // get file descriptor from args
    fd = atoi(argv[1]); 
    printf ("recv fd %d (total %d)\n", fd, argc); 

    char tmp[32] = { 0 }; 
    int ret = read (fd, tmp, 31); 
    printf ("read %d, %s\n", ret, tmp); 
    close (fd); 
  }
  else 
  {
    int flag = O_RDONLY; 
#  ifdef CLOSE_ON_EXEC
    flag |= O_CLOEXEC; 
#  endif 
    fd = open ("/dev/random", flag); 
    if (fd == -1)
      err_sys ("open /dev/null"); 
    else 
      printf ("open /dev/null as %d\n", fd); 
      
    pid_t pid = 0;
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      char tmp[32] = { 0 }; 
      sprintf (tmp, "%d", fd); 
      execlp ("./exec_on_close", "./exec_on_close"/*argv[0]*/, tmp, NULL, (char *)NULL); 
      err_sys ("execlp error"); 
    }
    else 
    {
      printf ("fork and exec child %u\n", pid); 
      close (fd); 
    }
  }

  exit (0); 
}
