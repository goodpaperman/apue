#include "../apue.h" 
#include <fcntl.h> 
#include <strings.h> 
#include <errno.h> 

static void lockabyte (char const* name, int fd, off_t offset)
{
  if (writew_lock (fd, offset, SEEK_SET, 1) < 0)
    err_sys ("%s: writew_lock error %d", name, errno); 

  printf ("%s: got the lock, byte %ld\n", name, offset); 
}

int main (int argc, char *argv[])
{
  int fd; 
  pid_t pid; 

  if ((fd = creat ("templock", FILE_MODE) < 0))
    err_sys ("creat error"); 

  if (write (fd, "ab", 2) != 2)
    err_sys ("writer error"); 

  SYNC_INIT (); 
  if ((pid = fork ()) < 0)
    err_sys ("fork error"); 
  else if (pid == 0) {
    lockabyte ("child", fd, 0); 
    SYNC_TELL (getppid (), 0); 
    SYNC_WAIT (); 
    lockabyte ("child", fd, 1); 
  }
  else { 
    lockabyte ("parent", fd, 1); 
    SYNC_WAIT (); 
    SYNC_TELL(pid, 1); 
    lockabyte ("parent", fd, 0); 
  }

  sleep (2); 
  return 0; 
}
