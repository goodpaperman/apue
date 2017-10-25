#include "../apue.h" 
#include <errno.h> 

int main ()
{
  int ret = 0; 
  ret = system (0); 
  printf ("system return %d, exit code %d\n", ret); 

  ret = system ("date > date.log"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 

  ret = system ("top"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 

  ret = system ("nosuchcommand"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 

  ret = system ("who; exit 44"); 
  printf ("system return %d, errno = %d\n", ret, errno); 
  pr_exit (ret); 
  return 0; 
}
