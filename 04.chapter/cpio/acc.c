#include "../apue.h"
#include <fcntl.h> 
#include <strings.h> 


int main (int argc, char *argv[])
{
  if (argc != 3)
    err_quit ("usage: acc <pathname> <flag>"); 

  int flag = 0, oflag = 0; 
  if (strcasecmp (argv[2], "R") == 0)
  {
    flag = R_OK; 
    oflag = O_RDONLY; 
  }
  else if (strcasecmp (argv[2], "W") == 0)
  {
    flag = W_OK; 
    oflag = O_WRONLY; 
  }
  else if (strcasecmp (argv[2], "X") == 0)
  {
    flag = X_OK; 
    oflag = O_RDWR; 
  }
  else if (strcasecmp (argv[2], "F") == 0)
  {
    flag = F_OK; 
    oflag = O_RDONLY; 
  }
  else 
  {
    printf ("unknown flag %s\n", argv[2]); 
    exit (0); 
  }

  if (access (argv[1], flag) < 0)
    err_ret ("access error for %s", argv[1]); 
  else 
    printf ("%s access OK\n", argv[2]); 

  int fd = open (argv[1], oflag); 
  if (fd < 0)
    err_ret ("open error for %s", argv[1]); 
  else 
    printf ("open for %sing OK\n", argv[2]); 

  close (fd); 
  exit (0); 
}
