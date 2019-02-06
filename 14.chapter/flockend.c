#include "../apue.h" 
#include <fcntl.h> 
#include <strings.h> 
#include <errno.h> 
#include <limits.h>

void usage ()
{
    printf (
      "Usage:\n"
      "flockend path\n"
      ); 
    exit (-1); 
}

void dump_lock (char const* act, struct flock *lck)
{
  printf ("[%lu] %s %s (%d, %d) @ %s\n", 
      getpid (), 
      act, 
      lck->l_type == F_RDLCK 
        ? "rdlock" 
        : (lck->l_type == F_WRLCK 
          ? "wrlock" 
          : "unlock"), 
      lck->l_len > 0 
        ? lck->l_start 
        : lck->l_start + lck->l_len, 
      lck->l_len > 0 
        ? lck->l_start + lck->l_len 
        : lck->l_len == 0 
          ? INT_MAX 
          : lck->l_start, 
      lck->l_whence == SEEK_SET 
        ? "beg" 
        : (lck->l_whence == SEEK_CUR 
          ? "cur" 
          : "end")); 
}

int main (int argc, char *argv[])
{
  if (argc != 2)
    usage (); 

  int fd = open (argv[1], O_RDWR | O_APPEND); 
  if (fd == -1)
    err_sys ("open %s failed", argv[1]); 

  //printf ("%lu start\n", getpid ()); 
  struct flock lock; 
  lock.l_type = F_RDLCK; 
  lock.l_start = 0; 
  lock.l_whence = SEEK_END; 
  lock.l_len = 0; 
  dump_lock ("LOCK", &lock); 
  int ret = fcntl (fd, F_SETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  printf ("[%lu] got lock\n", getpid ()); 
  sleep (3); 
  
  ret = write (fd, "helo", 1); 
  printf ("[%lu] increase file by write %d byte\n", getpid (), ret); 

  lock.l_type = F_UNLCK; 
  lock.l_start = 0; 
  lock.l_whence = SEEK_END; 
  lock.l_len = 0; 
  dump_lock ("UNLOCK", &lock); 
  ret = fcntl (fd, F_SETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  printf ("[%lu] release lock\n", getpid ()); 
  sleep (2); 

  printf ("%lu exit\n", getpid ()); 
}
