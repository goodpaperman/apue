#include "../apue.h"
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

int main (int argc, char *argv[])
{
#if defined (USE_CLOEXEC)
  int fd = open ("file.lk", O_RDWR | O_CREAT | O_CLOEXEC, 0644); 
#else 
  int fd = open ("file.lk", O_RDWR | O_CREAT /*| O_CLOEXEC*/, 0644); 
#endif 
  if (fd == -1)
    err_sys ("open file.lk"); 

  int val = 0; 
  if ((val = fcntl (fd, F_GETFD, 0)) < 0)
    err_sys ("fcntl error for fd %d", fd); 

  //printf ("fd flags: 0x%x\n", val); 
  if (val & FD_CLOEXEC)
    printf ("fd %d: close_on_exec\n", fd); 

  char const* ptr = "hello world!";  
  write (fd, ptr, strlen (ptr)); 
  printf ("open & write to fd %d\n", fd);

  char arg[128] = { 0 }; 
  sprintf (arg, "%d", fd); 
  char *target = "cloexec_i"; 
  char *args[3] = { target, &arg[0], 0 }; 
  char *env[1] = { 0 }; 
  execve (target, args, env); 
  err_sys ("execve"); 
  close (fd); 
  return 0; 
}
