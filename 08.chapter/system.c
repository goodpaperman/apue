#include "../apue.h" 
#include <errno.h> 

#if 0
#  define SYSTEM system
#else 
extern int my_system (const char *cmdstring); 
typedef int (*MY_SYSTEM) (const char *cmdstring); 
MY_SYSTEM SYSTEM = my_system; 
#endif 

int main (int argc, char *argv[])
{
  int ret = 0; 
  ret = SYSTEM (0); 
  printf ("system return %d\n", ret); 

  ret = SYSTEM ("date > date.log"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 

  ret = SYSTEM ("top"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 

  ret = SYSTEM ("nosuchcommand"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 

  ret = SYSTEM ("who; exit 44"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 

  if (argc > 1) 
  {
    ret = SYSTEM (argv[1]); 
    printf ("system return %d, errno = %d\n", ret, errno); 
    pr_exit (ret); 
  }
  return 0; 
}
