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

int main (int argc, char *argv[])
{
  printf ("in child %u\n", getpid ()); 
  if (argc < 2)
  {
    print_ids (); 
    return -1; 
  }

  //print_ids (); 
  //return 0; 
  

  int ret = 0; 
  char const* lock = argv[1]; 
  int uid = getuid (); 
  int euid = geteuid (); 
  print_ids (); 

  ret = setreuid (euid, uid); 
  if (ret == -1)
    err_sys ("setreuid"); 

  printf ("setuid back to %d.%d\n", euid, uid); 
  print_ids (); 


  int fd = open (lock, O_RDWR); 
  if (fd == -1)
    err_sys ("open"); 

  printf ("open lock file : %d\n", fd); 
  ret = flock (fd, LOCK_EX); 
  if (ret == -1)
    err_sys ("flock"); 

  printf ("lock file\n"); 
  ret = flock (fd, LOCK_UN); 
  if (ret == -1)
    err_sys ("funlock"); 

  close (fd); 
  printf ("uucp exit \n"); 
  return 0; 
}


