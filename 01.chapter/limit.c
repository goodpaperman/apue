//#include <limit.h> 
//#include <sys/time.h> 
#include <sys/resource.h> 
#include <stdio.h> 
#include <errno.h> 


typedef struct enum_name 
{
  int val; 
  char const* name; 
} enum_name_t ; 


#define N 15
#define DEFINE_ELEM(X) {X, #X}, 
int main (int argc, char *argv[])
{
  int ret = 0; 
  struct rlimit lm = { 0 }; 
  enum_name_t res[N] = 
  {
    DEFINE_ELEM(RLIMIT_AS) 
    DEFINE_ELEM(RLIMIT_CORE) 
    DEFINE_ELEM(RLIMIT_CPU) 
    DEFINE_ELEM(RLIMIT_DATA) 
    DEFINE_ELEM(RLIMIT_FSIZE) 
    DEFINE_ELEM(RLIMIT_LOCKS) 
    DEFINE_ELEM(RLIMIT_MEMLOCK) 
    DEFINE_ELEM(RLIMIT_MSGQUEUE) 
    DEFINE_ELEM(RLIMIT_NICE) 
    DEFINE_ELEM(RLIMIT_NOFILE) 
    DEFINE_ELEM(RLIMIT_NPROC) 
    DEFINE_ELEM(RLIMIT_RSS) 
    DEFINE_ELEM(RLIMIT_RTPRIO) 
    DEFINE_ELEM(RLIMIT_SIGPENDING) 
    DEFINE_ELEM(RLIMIT_STACK) 
  }; 
  

  int i = 0; 
  for (i=0; i<N; ++ i)
  {
    ret = getrlimit (res[i].val, &lm); 
    if (ret == -1)
      printf ("getrlimit %s failed, errno = %d\n", res[i].name, errno); 
    else 
      printf ("%s: soft %ld, hard %ld\n", res[i].name, lm.rlim_cur, lm.rlim_max); 
  }

  return 0; 
}
