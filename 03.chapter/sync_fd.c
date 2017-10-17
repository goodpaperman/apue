#include "../apue.h"
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

int main (int argc, char *argv[])
{
  int fd = open ("file.log", O_WRONLY | O_CREAT | O_TRUNC, 0644); 
  if (fd == -1)
    err_sys ("open file.log"); 

  int ret = 0, n = 0; 
  char const* ptr = "hello world!";  
  while (n++ < 1000)
  //while (1)
  {
    ret = write (fd, ptr, strlen (ptr)); 
    printf ("%5d:write %d\n", n, ret);
    if (ret < 0)
      err_sys ("write file.log"); 

    usleep (10000);  // 100 milliseconds
    //lseek (fd, 0, SEEK_SET); 
    //fsync (fd); 
    fdatasync (fd); 
  }

  close (fd); 
  return 0; 
}
