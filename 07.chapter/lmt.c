#include "../apue.h"
#include <errno.h>
//#include <string.h> 
#include <sys/time.h> 
#include <sys/resource.h> 
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <mqueue.h> 

#ifdef TEST_LIMIT_CPU
void sigxcpu_handler (int sig)
{
  printf ("ate SIGXCPU...\n"); 
  signal (SIGXCPU, sigxcpu_handler); 
}
#endif

#ifdef TEST_LIMIT_FSIZE
void sigxfsz_handler (int sig)
{
  printf ("ate SIGXFSZ...\n"); 
  signal (SIGXFSZ, sigxfsz_handler); 
}
#endif

#ifdef TEST_LIMIT_STACK
int stack_depth = 0; 
void call_stack_recur ()
{
  char buf[1024] = { 0 }; 
  printf ("call stack %d\n", stack_depth++); 
  call_stack_recur (); 
}
#endif 

int main (int argc, char *argv[])
{
  int ret = 0; 
  struct rlimit lmt = { 0 }; 
#if defined(TEST_LIMIT_AS) || defined (TEST_LIMIT_DATA)
  lmt.rlim_cur = 1024 * 1024; 
  lmt.rlim_max = RLIM_INFINITY; 
#  ifdef TEST_LIMIT_AS
  ret = setrlimit (RLIMIT_AS, &lmt);  
#  else
  ret = setrlimit (RLIMIT_DATA, &lmt); 
#  endif 
  if (ret == -1)
    err_sys ("set rlimit as/data failed"); 
  char *ptr = malloc (1024 * 1024); 
  if (ptr == NULL)
    err_sys ("malloc failed"); 
  printf ("alloc 1 MB success!\n"); 
  free (ptr); 
#endif 

#ifdef TEST_LIMIT_CORE
#  if 1
  lmt.rlim_cur = 1; 
  lmt.rlim_max = 1024; 
  ret = setrlimit (RLIMIT_CORE, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit core failed"); 
#  endif 
  char *ptr = 0; 
  *ptr = 1; 
#endif 

#ifdef TEST_LIMIT_CPU
  signal (SIGXCPU, sigxcpu_handler); 

  lmt.rlim_cur = 1; 
  lmt.rlim_max = 5; 
  ret = setrlimit (RLIMIT_CPU, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit cpu failed"); 

  int i = 1, j = 1; 
  while (1)
  {
    i *= j++; 
  }
#endif 

#ifdef TEST_LIMIT_FSIZE
  signal (SIGXFSZ, sigxfsz_handler); 

  lmt.rlim_cur = 1024; 
  lmt.rlim_max = RLIM_INFINITY; 
  ret = setrlimit (RLIMIT_FSIZE, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit fsize failed"); 

  //FILE* fp = fopen ("core.tmp", "w+"); 
  //if (fp == NULL)
  //  err_sys ("open file failed"); 
  int fd = open ("core.tmp", O_RDWR | O_CREAT, 0644); 
  if (fd == -1)
    err_sys ("open file failed"); 

  char buf[16]; 
  int total = 0; 
  while (1)
  {
    //ret = fwrite (buf, 16, 1, fp); 
    ret = write (fd, buf, 16); 
    if (ret == -1)
      err_sys ("write failed"); 

    total += ret; 
    printf ("write %d, total %d\n", ret, total); 

  }

  //fclose (fp); 
  close (fd); 
#endif 

#ifdef TEST_LIMIT_LOCKS
#  if 1
  lmt.rlim_cur = 1; 
  lmt.rlim_max = 1; 
  ret = setrlimit (RLIMIT_LOCKS, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit locks failed"); 
#  endif

#define FD_SIZE 10 
  char filename[256] = { 0 }; 
  int fds[FD_SIZE] = { 0 }; 
  for (int i=0; i<FD_SIZE; ++ i)
  {
    sprintf (filename, "core.%02d.lck", i+1); 
    fds[i] = open (filename, O_RDWR | O_CREAT, 0644); 
    if (fds[i] == -1)
      err_sys ("open file failed"); 

    ret = flock (fds[i], LOCK_EX /*| LOCK_NB | LOCK_SH*/); 
    if (ret == -1)
      err_sys ("lock file failed"); 

    printf ("establish lock %2d OK\n", i+1); 
  }

  for (int i=0; i<FD_SIZE; ++ i)
  {
    if (fds[i] != 0)
    {
      //flock (fds[i], LOCK_UN); 
      close (fds[i]); 
    }
  }
#endif 

#ifdef TEST_LIMIT_MEMLOCK
#  if 1
  lmt.rlim_cur = 1024; 
  lmt.rlim_max = 1024; 
  ret = setrlimit (RLIMIT_MEMLOCK, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit memlock failed"); 
#  endif 
  char *ptr = malloc (1024 * 1024); 
  if (ptr == NULL)
    err_sys ("malloc failed"); 
   
  printf ("alloc 1 MB success!\n"); 
  ret = mlock (ptr, 1024 * 1024); 
  if (ret == -1)
    err_sys ("mlock failed, %d", errno); 

  printf ("lock 1 MB success!\n"); 
  munlock (ptr, 1024 * 1024); 
  free (ptr); 
#endif 

#ifdef TEST_LIMIT_MSGQUEUE
#  if 1
  lmt.rlim_cur = 0; 
  lmt.rlim_max = 0; 
  ret = setrlimit (RLIMIT_MSGQUEUE, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit msgqueue failed"); 
#  endif 

#  define MAXMSG 9
  struct mq_attr attr = { 0 }; 
  attr.mq_flags = O_NONBLOCK; 
  attr.mq_maxmsg = MAXMSG; 
  attr.mq_msgsize = 1024/MAXMSG+1; 
  attr.mq_curmsgs = 0; 
  mqd_t que = mq_open ("/test", O_RDWR | O_CREAT | O_NONBLOCK/*| O_EXCL*/, 0644, &attr); 
  if (que == -1)
    err_sys ("mq_open failed with size %d", attr.mq_maxmsg * attr.mq_msgsize); 

  char ch = 0; 
  printf ("open queue OK\n"); 
  for (int i=0; i<MAXMSG; ++ i)
  {
    ch = i; 
    ret = mq_send (que, &ch, sizeof (ch), i); 
    if (ret == -1)
    {
      err_ret ("send msg failed"); 
      break; 
    }
  }

  printf ("send %d msgs\n", MAXMSG); 
  mq_close (que);
#endif 

#ifdef TEST_LIMIT_NOFILE
#  if 1
  lmt.rlim_cur = 5; 
  lmt.rlim_max = 5; 
  ret = setrlimit (RLIMIT_NOFILE, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit nofile failed"); 
#  endif

#define FD_SIZE 10 
  char filename[256] = { 0 }; 
  int fds[FD_SIZE] = { 0 }; 
  for (int i=0; i<FD_SIZE; ++ i)
  {
    sprintf (filename, "core.%02d.lck", i+1); 
    fds[i] = open (filename, O_RDWR | O_CREAT, 0644); 
    if (fds[i] == -1)
      err_sys ("open file failed"); 

    printf ("open file %s\n", filename); 
  }

  for (int i=0; i<FD_SIZE; ++ i)
  {
    if (fds[i] != 0)
    {
      close (fds[i]); 
    }
  }
#endif 

#ifdef TEST_LIMIT_NPROC
#  if 1
  lmt.rlim_cur = 65; 
  lmt.rlim_max = 65; 
  ret = setrlimit (RLIMIT_NPROC, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit nproc failed"); 
#  endif

#define PROC_SIZE 10 
  int pids[PROC_SIZE] = { 0 }; 
  for (int i=0; i<PROC_SIZE; ++ i)
  {
    pids[i] = fork (); 
    if (pids[i] == -1)
      err_sys ("fork failed"); 
    if (pids[i] == 0)
    {
      printf ("child %d running\n", getpid ()); 
      sleep (1);
      break; 
    }

    printf ("create child %d\n", pids[i]); 
  }

  sleep (1);
#endif 

#ifdef TEST_LIMIT_STACK
#  if 1
  lmt.rlim_cur = 1024 * 10; 
  lmt.rlim_max = 1024 * 10; 
  ret = setrlimit (RLIMIT_STACK, &lmt);  
  if (ret == -1)
    err_sys ("set rlimit stack failed"); 
#  endif

  call_stack_recur (); 
#endif 

  return 0; 
}
