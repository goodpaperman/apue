#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <dirent.h> 

char *str = 0; 
int main (int argc, char *argv[])
{
  int fd = 0; 
  DIR *dir = 0; 
  int flag = 0; 
  if (argc > 1)
  {
    // child mode
    // get file descriptor from args
    fd = atol(argv[1]); 
    char *s = argv[2]; 

    flag = fcntl (fd, F_GETFD); 
    printf ("dir fd(%d) flag: 0x%x, CLOSE_ON_EXEC: %d\n", fd, flag, flag & FD_CLOEXEC); 

    dir = fdopendir(fd); 
    printf ("recv dir %d, str %s (total %d)\n", fd, s, argc); 

    flag = fcntl (fd, F_GETFD); 
    printf ("dir fd(%d) flag: 0x%x, CLOSE_ON_EXEC: %d\n", fd, flag, flag & FD_CLOEXEC); 

    rewinddir(dir); 
    struct dirent *ent = readdir (dir); 
    printf ("read %p, %s\n", ent, ent ? ent->d_name : 0); 
    closedir (dir); 
  }
  else 
  {
    str = strdup ("hello world"); 
#ifdef USE_FDOPENDIR
    fd = open(".", O_RDONLY); 
    if (fd == -1)
        err_sys ("open"); 

    flag = fcntl (fd, F_GETFD); 
    printf ("dir fd(%d) flag: 0x%x, CLOSE_ON_EXEC: %d\n", fd, flag, flag & FD_CLOEXEC); 

    // close (fd); 
    dir = fdopendir (fd); 
#else 
    dir = opendir ("."); 
    if (dir == NULL)
      err_sys ("open ."); 
    else 
      printf ("open . return %p\n", dir); 
      
    fd = dirfd (dir);
#endif

    flag = fcntl (fd, F_GETFD); 
    printf ("dir fd(%d) flag: 0x%x, CLOSE_ON_EXEC: %d\n", fd, flag, flag & FD_CLOEXEC); 

    // restore CLOSE_ON_EXEC
    fcntl (fd, F_SETFD, flag & ~FD_CLOEXEC); 

    flag = fcntl (fd, F_GETFD); 
    printf ("dir fd(%d) flag: 0x%x, CLOSE_ON_EXEC: %d\n", fd, flag, flag & FD_CLOEXEC); 

    pid_t pid = 0;
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      char tmp[32] = { 0 }; 
      sprintf (tmp, "%lu", (long)fd); 

      execlp ("./exec_open_dir", "./exec_open_dir"/*argv[0]*/, tmp, str, NULL); 
      err_sys ("execlp error"); 
    }
    else 
    {
      printf ("fork and exec child %u\n", pid); 
      struct dirent *ent = readdir (dir); 
      printf ("read %p, %s\n", ent, ent ? ent->d_name : 0); 

      // closedir (dir); 
    }
  }

  exit (0); 
}
