#include <sys/utsname.h> 
#include "../apue.h"

int main (int argc, char *argv[])
{
  struct utsname uts = {{ 0 }}; 
  printf ("sizeof (struct utsname) = %d\n", sizeof(uts)); 
  int ret = uname (&uts); 
  if (ret == -1)
  {
    perror ("uname"); 
    return -1; 
  }

  printf ("sysname: %s\n", uts.sysname); 
  printf ("nodename: %s\n", uts.nodename); 
  printf ("release: %s\n", uts.release); 
  printf ("version: %s\n", uts.version); 
  printf ("machine: %s\n", uts.machine); 
#ifdef _GNU_SOURCE
  printf ("domain: %s\n", uts.domainname); 
#endif 
  return 0; 
}
