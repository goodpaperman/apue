#include "../apue.h" 
#include <sys/wait.h> 
#include <sys/resource.h>

int main (int argc, char *argv[])
{
#if 1
  pid_t pid = 0; 
  if ((pid = fork ()) < 0)
    err_sys ("fork error"); 
  else if (pid == 0)
    printf ("this is child %d\n", getpid ()); 
  else
    printf ("before wait any child\n"); 
#endif

  pid_t cld = 0; 
  int status = 0; 
#ifdef USE_WAITID
  siginfo_t sig = { 0 }; 
  int flag = WEXITED | WCONTINUED | WSTOPPED; 
  cld = waitid (P_ALL, -1, &sig, flag); 
#elif USE_WAIT_RUSAGE
  int flag = WUNTRACED | WCONTINUED; 
  struct rusage ru = { 0 }; 
  cld = wait3 (&status, flag, &ru); 
#elif USE_WAITPID
  int flag = WUNTRACED | WCONTINUED; 
  cld = waitpid (-1, &status, flag); 
#else
  cld = wait (&status); 
#endif
  if (cld == -1)
  {
    err_msg ("wait[p][id] failed"); 
    sleep (20); 
    return -1; 
  }
  else
  {
#ifdef USE_WAITID
    printf ("wait child %d\n", sig.si_pid); 
    switch (sig.si_code)
    {
      case CLD_EXITED:
        printf ("exit code = %d\n", sig.si_status); 
        break; 
      case CLD_KILLED:
        printf ("killed by %d\n", sig.si_status); 
        break; 
      case CLD_DUMPED:
        printf ("killed and dumped by %d\n", sig.si_status); 
        break; 
      case CLD_STOPPED:
        printf ("stopped by %d\n", sig.si_status); 
        break; 
      case CLD_CONTINUED:
        printf ("continued by %d\n", sig.si_status); 
        break; 
      case CLD_TRAPPED:
        printf ("trapped by %d\n", sig.si_status); 
        break; 
      default:
        printf ("unknown code %d\n", sig.si_code); 
        break; 
    }
#else
    printf ("wait child %d, ret = 0x%x\n", cld, status); 
    if (WIFEXITED(status))
      printf ("exit code = %d\n", WEXITSTATUS(status)); 
    else if (WIFSIGNALED(status))
#  ifdef WCOREDUMP
      printf ("signaled by %d, has core dump ? %s\n", 
        WTERMSIG(status), 
        WCOREDUMP(status) ? "true" : "false"); 
#  else
      printf ("signaled by %d\n", WTERMSIG(status)); 
#  endif
    else if (WIFSTOPPED(status))
      printf ("stopped by %d\n", WSTOPSIG(status)); 
    else if (WIFCONTINUED(status))
      printf ("contined\n"); 
    else 
      printf ("unknow process status\n");  

#  ifdef USE_WAIT_RUSAGE
    printf ("rusage :\n"
      "user time: %lu.%lu\n"
      "sys time: %lu.%lu\n"
      "max rss: %ld\n"
      "shr rss: %ld\n"
      "unshr rss: %ld\n"
      "unshr stk: %ld\n"
      "min flt: %ld\n"
      "maj flt: %ld\n"
      "swap: %ld\n"
      "in blk: %ld\n"
      "out blk: %ld\n"
      "msg snd: %ld\n"
      "msg rcv: %ld\n"
      "sig: %ld\n"
      "vcsw: %ld\n"
      "ivcsw: %ld\n", 
      ru.ru_utime.tv_sec, 
      ru.ru_utime.tv_usec, 
      ru.ru_stime.tv_sec, 
      ru.ru_stime.tv_usec, 
      ru.ru_maxrss, 
      ru.ru_ixrss, 
      ru.ru_idrss, 
      ru.ru_isrss, 
      ru.ru_minflt, 
      ru.ru_majflt, 
      ru.ru_nswap, 
      ru.ru_inblock, 
      ru.ru_oublock, 
      ru.ru_msgsnd, 
      ru.ru_msgrcv, 
      ru.ru_nsignals, 
      ru.ru_nvcsw, 
      ru.ru_nivcsw); 
#  endif
#endif
  }

  return 0; 
}
