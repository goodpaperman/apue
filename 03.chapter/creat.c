#include <stdio.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 

int main (int argc, char *argv[])
{
  int ret = 0; 
  int fd = creat ("test.log", 0644); 
  if (fd == -1)
  {
    printf ("creat failed, errno = %d\n", errno); 
    return -1; 
  }

  char buf[32] = { 0 }; 
  ret = read (fd, buf, 31); 
  printf ("read return %d, read : %s, errno = %d\n", ret, buf, errno); 
  close (fd); 
  return 0; 
}
