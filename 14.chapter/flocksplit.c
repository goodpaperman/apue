#include "../apue.h" 
#include <fcntl.h> 
#include <strings.h> 
#include <errno.h> 

void usage ()
{
    printf (
      "Usage:\n"
      "flocksplit path len\n"
      ); 
    exit (-1); 
}

void dump_lock (char const* act, struct flock *lck)
{
  printf ("[%lu] %s %s (%u, %u) @ %s\n", getpid (), act, lck->l_type == F_RDLCK ? "rdlock" : (lck->l_type == F_WRLCK ? "wrlock" : "unlock"), lck->l_len > 0 ? lck->l_start : lck->l_start + lck->l_len, lck->l_len > 0 ? lck->l_start + lck->l_len : lck->l_start - 1, lck->l_whence == SEEK_SET ? "beg" : (lck->l_whence == SEEK_CUR ? "cur" : "end")); 
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

  printf ("[%lu] got lock\n", getpid ()); 
  // note: GETLK can NOT test lock hold by process itself !
#if 0
  lock.l_type = F_RDLCK; 
  lock.l_start = 0; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = len; 
  dump_lock ("TESTLOCK", &lock); 
  ret = fcntl (fd, F_GETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  if (lock.l_type == F_UNLCK)
    printf ("no lock in destination found\n"); 
  else 
  {
    printf ("find a lock owning by %lu\n", getpid()); 
    dump_lock ("GETLOCK", &lock); 
  }
#else
  sleep (2); 
#endif

  lock.l_type = F_UNLCK; 
  lock.l_start = len/2; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = 1; 
  dump_lock ("UNLOCK", &lock); 
  ret = fcntl (fd, F_SETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  printf ("[%lu] release lock\n", getpid ()); 
#if 0
  lock.l_type = F_RDLCK; 
  lock.l_start = 0; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = len; 
  dump_lock ("TESTLOCK", &lock); 
  ret = fcntl (fd, F_GETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  if (lock.l_type == F_UNLCK)
    printf ("no lock in destination found\n"); 
  else 
  {
    printf ("find a lock owning by %lu\n", getpid()); 
    dump_lock ("GETLOCK", &lock); 
  }
#else
  sleep (2); 
#endif

  lock.l_type = F_RDLCK; 
  lock.l_start = len/2; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = 1; 
  dump_lock ("LOCK", &lock); 
  ret = fcntl (fd, F_SETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  printf ("[%lu] got lock again\n", getpid ()); 
#if 0
  lock.l_type = F_RDLCK; 
  lock.l_start = 0; 
  lock.l_whence = SEEK_SET; 
  lock.l_len = len; 
  dump_lock ("TESTLOCK", &lock); 
  ret = fcntl (fd, F_GETLK, &lock); 
  if (ret == -1)
    err_sys ("fcntl failed, errno %d", errno); 

  if (lock.l_type == F_UNLCK)
    printf ("no lock in destination found\n"); 
  else 
  {
    printf ("find a lock owning by %lu\n", getpid()); 
    dump_lock ("GETLOCK", &lock); 
  }
#else
  sleep (2); 
#endif

  printf ("%lu exit\n", getpid ()); 
}
