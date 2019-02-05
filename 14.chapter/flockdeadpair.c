#include "../apue.h" 
#include <fcntl.h> 
#include <strings.h> 
#include <errno.h> 

static void lockabyte (char const* name, int fd, off_t offset)
{
  if (writew_lock (fd, offset, SEEK_SET, 1) < 0)
    err_sys ("%s: writew_lock error %d", name, errno); 

  printf ("%s: got the lock on fd %d, byte %ld\n", name, fd, offset); 
}

int main (int argc, char *argv[])
{
  int fd; 
  if (argc != 3)
  {
    printf ("Usage: flockdeadpair path child(0|1)\n"); 
    exit (-1); 
  }

  if ((fd = open (argv[1], O_RDWR)) < 0)
    err_sys ("open error"); 

  //if (write (fd, "ab", 2) != 2)
  //  err_sys ("writer error"); 

  if (atoi(argv[2]) == 1) {
    lockabyte ("child", fd, 0); 
    sleep (1); 
    lockabyte ("child", fd, 1); 
  }
  else { 
    lockabyte ("parent", fd, 1); 
    sleep (1); 
    lockabyte ("parent", fd, 0); 
  }

  sleep (1); 
  return 0; 
}
