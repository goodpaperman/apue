#include "../apue.h"
#include <sys/types.h> 
#include <sys/file.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <fcntl.h> 

void print_ids ()
{
  int uid = getuid (); 
  int euid = geteuid (); 
  printf ("uid = %d\n", uid); 
  printf ("euid = %d\n", euid); 
}

void create_uucp (char const* lock)
{
  int pid = fork (); 
  if (pid < 0)
    err_sys ("fork"); 
  else if (pid == 0)
  {
    // children 
    execlp ("./uucp", "./uucp", lock, NULL); 
    err_sys ("execlp"); 
  }
  else 
    printf ("create uucp %u\n", pid); 
}

int main (int argc, char *argv[])
{
  if (argc < 2)
  {
    printf ("Usage: tip lockfile\n"); 
    print_ids (); 
    return -1; 
  }

  int ret = 0; 
  char const* lock = argv[1]; 
  int uid = getuid (); 
  int euid = geteuid (); 
  print_ids (); 

  int fd = open (lock, O_RDWR); 
  if (fd == -1)
    err_sys ("open"); 

  printf ("open lock file : %d\n", fd); 
  ret = flock (fd, LOCK_EX); 
  if (ret == -1)
    err_sys ("flock"); 

  printf ("lock file\n"); 
  sleep (10); 
  ret = setuid (uid); 
  if (ret == -1)
    err_sys ("setuid"); 

  printf ("setuid back to %d\n", uid); 
  print_ids (); 

  printf ("do something...\n"); 
  create_uucp (lock); 
  sleep (10); 

  ret = setuid (euid); 
  if (ret == -1)
    err_sys ("setuid"); 

  printf ("setuid to %d\n", euid); 
  print_ids (); 

  ret = flock (fd, LOCK_UN); 
  if (ret == -1)
    err_sys ("funlock"); 

  close (fd); 
  sleep (10); 
  return 0; 
}


