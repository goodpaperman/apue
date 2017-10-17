#include "../apue.h"
#include <linux/limits.h> 
#include <fcntl.h> 
#include <pwd.h> 
#include <errno.h> 
#include <dirent.h> 

int main (int argc, char *argv[])
{
  //printf ("test dir mode on : %s\n", argv[1]); 

  // 1. read dir files
  DIR *dir = opendir (argv[1]); 
  if (dir)
  {
    struct dirent* ent = 0; 
    while ((ent = readdir (dir)))
    {
      printf ("read %ld: %s\n", ent->d_ino, ent->d_name); 
    }

    closedir (dir); 
  }
  else 
    printf ("opendir %s failed, errno %d\n", argv[1], errno); 

  char path[PATH_MAX+1] = { 0 }; 
  strcpy (path, argv[1]); 
  strcat (path, "/"); 
  strcat (path, argv[2]); 
  // 1.5 open file
  int fd = 0; 
  if ((fd = open (path, O_RDONLY)) != -1)
  {
    printf ("open file %s OK\n", path); 
    close (fd); 
  }
  else 
    printf ("open file %s failed, errno %d\n", path, errno); 

  strcpy (path, argv[1]); 
  strcat (path, "/"); 
  strcat (path, "abc"); 

  // 2. create file
  if ((fd = creat (path, 0755)) != -1)
  {
    printf ("create file %s OK\n", path); 
    close (fd); 
  }
  else
    printf ("create file %s failed, errno %d\n", path, errno); 

  // 3. delete file
  if (unlink (path) == 0)
    printf ("delete file %s OK\n", path); 
  else 
    printf ("delete file %s failed, errno %d\n", path, errno); 

  // 4. change dir
  if (chdir (argv[1]) == 0)
    printf ("chdir %s OK\n", argv[1]); 
  else 
    printf ("chdir %s failed, errno %d\n", argv[1], errno); 

  printf ("--------------------------\n"); 
  return 0; 
}
