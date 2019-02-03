#include "../apue.h" 
#include <fcntl.h> 
#include <strings.h> 
#include <errno.h> 

void usage ()
{
    printf (
      "Usage:\n"
      "flockcover path len\n"
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
        : lck->l_start, 
      lck->l_whence == SEEK_SET 
        ? "beg" 
        : (lck->l_whence == SEEK_CUR 
          ? "cur" 
          : "end")); 
}

int main (int argc, char *argv[])
{
  if (argc != 3)
    usage (); 

  off_t len = atoi(argv[2]); 
  int fd = open (argv[1], O_RDWR); 
  if (fd == -1)
    err_sys ("open %s failed", argv[1]); 

  //printf ("%lu start\n", getpid ()); 
  struct flock lock; 
  lock.l_type = F_RDLCK; 
  lock.l_start = 0; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = len; 
  dump_lock ("LOCK", &lock); 
  int ret = fcntl (fd, F_SETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  printf ("[%lu] got read lock\n", getpid ()); 
  sleep (2); 

  lock.l_type = F_WRLCK; 
  lock.l_start = len/4; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = len/2; 
  dump_lock ("LOCK", &lock); 
  ret = fcntl (fd, F_SETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  printf ("[%lu] got write lock\n", getpid ()); 
  sleep (2); 

  lock.l_type = F_RDLCK; 
  lock.l_start = len/2; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = 1; 
  dump_lock ("LOCK", &lock); 
  ret = fcntl (fd, F_SETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  printf ("[%lu] got read lock again\n", getpid ()); 
  sleep (2); 

  printf ("%lu exit\n", getpid ()); 
}
