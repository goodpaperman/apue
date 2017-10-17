#include "../apue.h"
#include <fcntl.h> 
#include <pwd.h> 
#include <errno.h> 

int main (int argc, char *argv[])
{
  if (mkdir (argv[1], /* 0666 */ 0755) < 0) 
    err_sys ("%s: mkdir error", argv[1]); 

  printf ("mkdir %s\n", argv[1]); 

#if 0
  struct passwd *pwd = 0; 
  pwd = getpwnam ("yh"); 
  int uid = pwd->pw_uid; 
  printf ("uid of 'yh' is %d\n", uid); 
  if (chown (argv[1], uid, uid) < 0)
    err_sys ("%s: chown error", argv[1]); 

  printf ("chown %s\n", argv[1]); 
#endif 

  int ret = fork (); 
  if (ret < 0)
    err_sys ("fork error"); 
  else if (ret == 0)
  {
    // child
    if (chdir (argv[1]) < 0)
      err_sys ("chdir error"); 

    printf ("chdir OK\n"); 
    sleep (5); 
    if (creat ("xyz.txt", 0755) < 0)
      printf ("create xyz.txt failed, errno = %d\n", errno); 
    else 
      printf ("create xyz.txt\n"); 

    sleep (3); 
    printf ("child quit\n"); 
    exit (0); 
  }
  else 
  {
    // parent
    sleep (3); 
  }

#if 1
  if (rmdir (argv[1]) < 0)
    err_sys ("%s: rmdir error", argv[1]); 

  printf ("rmdir %s\n", argv[1]); 
#endif 
  return 0; 
}
