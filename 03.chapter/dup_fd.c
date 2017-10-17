#include "../apue.h"
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

int main (int argc, char *argv[])
{
  int fd = open ("file.dup", O_RDWR | O_CREAT | O_CLOEXEC, 0644); 
  if (fd == -1)
    err_sys ("open file.dup"); 

  int val = 0; 
  printf ("fd = %d\n", fd); 
  if ((val = fcntl (fd, F_GETFD, 0)) < 0)
    err_sys ("fcntl error for fd %d", fd); 

  if (val & FD_CLOEXEC)
    printf ("fd %d: close_on_exec\n", fd); 

  int fd2 = 0; 
  if ((fd2 = dup2 (fd, 100)) < 0)
    err_sys ("dup2 error for fd %d", fd); 

  printf ("fd2 = %d\n", fd2); 
  if ((val = fcntl (fd2, F_GETFD, 0)) < 0)
    err_sys ("fcntl error for fd %d", fd2); 

  if (val & FD_CLOEXEC)
    printf ("fd2 %d: close_on_exec\n", fd2); 
    
  int fd3 = 0; 
#if 0
  if ((fd3 = fcntl (fd, F_DUPFD, 100)) < 0)
#else
  if ((fd3 = fcntl (fd, F_DUPFD_CLOEXEC, 100)) < 0)
#endif
    err_sys ("fcntl dup error for fd %d", fd3); 

  printf ("fd3 = %d\n", fd3); 
  if ((val = fcntl (fd3, F_GETFD, 0)) < 0)
    err_sys ("fcntl error for fd %d", fd3); 

  if (val & FD_CLOEXEC)
    printf ("fd3 %d: close_on_exec\n", fd3); 

  char const* ptr = "hello world!";  
  write (fd, ptr, strlen (ptr)); 
  printf ("open & write to fd %d\n", fd);

  lseek (fd2, 0, SEEK_SET); 
  printf ("seek to begin on fd %d\n", fd2); 

  char arg[128] = { 0 }; 
  read (fd3, arg, 127); 
  printf ("read '%s' on fd %d\n", arg, fd3); 

  close (fd); 
  close (fd2); 
  close (fd3); 
  return 0; 
}
