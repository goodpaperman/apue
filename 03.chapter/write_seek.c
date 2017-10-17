#include "../apue.h"
#include <stdio.h> 
#include <errno.h> 
#include <fcntl.h> 

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc < 2)
    err_sys ("Usage: write_inter file"); 

  int fd = open (argv[1], O_RDWR | O_CREAT | O_APPEND, 0644); 
  if (fd == -1)
    err_sys ("open failed"); 

  int n = 0; 
  int const size = 10; 
  char buf[size]; 
  strcpy (buf, "ABCDEFGHI"); 
  while (n++ < 10)
  {
    ret = write (fd, buf, size); 
    printf ("write %d\n", ret); 
    if (ret < size)
    {
      printf ("write less than require, errno = %d\n", errno); 
      break; 
    }

    ret = lseek (fd, 4, SEEK_SET); 
    printf ("lseek ret %d, errno = %d\n", ret, errno); 
  }

  ret = read (fd, buf, size-1); 
  buf[size] = 0; 
  printf ("read %d, %s\n", ret, buf); 
  close (fd); 
  return 0; 
}
