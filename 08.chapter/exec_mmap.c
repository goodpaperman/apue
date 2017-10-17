#include "../apue.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
//#include <pthread.h> 
#include <errno.h>
//#include <dirent.h> 
//#include <signal.h> 
#include <sys/mman.h> 

int main (int argc, char *argv[])
{
  int ret = 0; 
  if (argc > 1)
  {
    // child mode
    void *addr = 0; 
    //(void *)atol (argv[1]); 
    sscanf (argv[1], "%p", &addr); 
    printf ("[%d] argv[1] = %s, addr = %p\n", getpid (), argv[1], addr); 
    printf ("content: %s\n", (char *)addr); 
  }
  else 
  {
    int fd = open ("file.map", O_RDWR); 
    if (fd == -1)
      err_sys ("open"); 

    struct stat sb = { 0 }; 
    if (fstat (fd, &sb) == -1)
      err_sys ("fstat"); 

    off_t length = sb.st_size; 
    length = length & ~(sysconf(_SC_PAGE_SIZE) -1); 
    void *addr = mmap (NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if (addr == MAP_FAILED)
      err_sys ("mmap"); 

    strcpy (addr, "hello, world !"); 
    printf ("%p: %s\n", addr, (char *)addr); 

    pid_t pid = 0;
    if ((pid = fork ()) < 0)
      err_sys ("fork error"); 
    else if (pid == 0)
    {
      //printf ("content: %s\n", (char *)addr); 

      char tmp[32] = { 0 }; 
      sprintf (tmp, "%p", addr); 
      execlp ("./exec_mmap", "./exec_mmap"/*argv[0]*/, tmp, NULL); 
      err_sys ("execlp error"); 
    }
    else 
    {
      printf ("fork and exec child %u\n", pid); 
      sleep (2); 
      munmap (addr, length); 
      close (fd); 
    }
  }

  exit (0); 
}
