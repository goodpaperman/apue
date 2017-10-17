#include <unistd.h> 
//#include <limits.h> 
#include <bits/posix1_lim.h> 
#include "../apue.h"

int main (int argc, char *argv[])
{
  char host[HOST_NAME_MAX] = { 0 }; 
  int ret = gethostname (host, sizeof (host)); 
  if (ret == -1)
  {
    perror ("gethostname"); 
    return -1; 
  }

  printf ("hostname: %s\n", host); 
  return 0; 
}
