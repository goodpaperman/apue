#include "../apue.h"
#include <fcntl.h> 
#include <pwd.h> 
#include <errno.h> 
#include <dirent.h>
#include <linux/limits.h>

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
    char filename[PATH_MAX] = { 0 }; 
    strcpy (filename, "xyz.txt"); 
#if 0
    if (chdir (argv[1]) < 0)
      err_sys ("chdir error"); 

    printf ("chdir OK\n"); 
#else
    DIR* d = opendir(argv[1]); 
    if (d == NULL)
        err_sys ("opendir error"); 
    else 
        printf ("opendir\n"); 

    strcpy (filename, argv[1]); 
    strcat (filename, "/xyz.txt"); 
#endif

    sleep (5); 
    if (creat (filename, 0755) < 0)
      printf ("create %s failed, errno = %d\n", filename, errno); 
    else 
      printf ("create %s\n", filename); 

    closedir (d); 
    printf ("closedir \n"); 
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
