#include "../apue.h"
#include <errno.h>
#include <pwd.h>
#include <linux/limits.h>
#include <dirent.h> 

int main (int argc, char *argv[])
{
  if (argc != 3)
    err_quit ("usage: follow_links <file> <dir>"); 

  int fd = 0; 
  char const* file = argv[1]; 
  char const* dir = argv[2]; 

  if (access (file, F_OK) < 0)
    printf ("access error for '%s', errno = %d\n", file, errno);
  else 
    printf ("access OK for '%s'\n", file); 

  if (chdir (dir) < 0)
    printf ("chdir error for '%s', errno = %d\n", dir, errno); 
  else 
  {
    printf ("chdir OK for '%s'\n", dir); 
    // restore original base
    chdir (".."); 
  }

  if (chmod (file, 0777) < 0)
    printf ("chmod error for '%s', errno = %d\n", file, errno); 
  else 
  {
    printf ("chmod OK for '%s'\n", file); 
    // restore original permission
    //chmod (file, 0644); 
  }
  
  struct passwd *pwd = 0; 
  pwd = getpwnam ("yunhai"); 
  int uid = pwd->pw_uid; 
  printf ("uid of 'yunhai' is %d\n", uid); 
  if ((fd = chown (file, uid, uid)) < 0)
    printf ("chown error for '%s', errno = %d\n", file, errno); 
  else 
  {
    printf ("chown OK for '%s'\n", file); 
  }

  if ((fd = lchown (file, uid, uid)) < 0)
    printf ("lchown error for '%s', errno = %d\n", file, errno); 
  else 
  {
    printf ("lchown OK for '%s'\n", file); 
  }

  char path[PATH_MAX] = { 0 }; 
  strcpy (path, file); 
  strcat (path, ".2"); 
  if (link (file, path) < 0)
    printf ("link error for '%s', errno = %d\n", file, errno); 
  else 
  {
    printf ("link OK for '%s', new file '%s'\n", file, path); 
    //unlink (path); 
  }
  
  if (unlink (path) < 0)
    printf ("unlink error for '%s', errno = %d\n", path, errno); 
  else 
  {
    printf ("unlink OK for '%s'\n", path); 
    //creat (path); 
    //rename (path, file); 
  }
  
  struct stat statbuf = { 0 }; 
  if (stat (file, &statbuf) < 0)
    printf ("stat error for '%s', errno = %d\n", file, errno); 
  else 
    printf ("stat OK for '%s', mode=%d, ISLINK=%d\n", file, statbuf.st_mode, S_ISLNK(statbuf.st_mode)); 
  
  if (lstat (file, &statbuf) < 0)
    printf ("lstat error for '%s', errno = %d\n", file, errno); 
  else 
    printf ("lstat OK for '%s', mode=%d, ISLINK=%d\n", file, statbuf.st_mode, S_ISLNK(statbuf.st_mode)); 
  
  if ((fd = open (file, O_RDONLY)) < 0)
    printf ("open error for '%s', errno = %d\n", file, errno); 
  else 
  {
    printf ("open OK for '%s'\n", file); 
    close (fd); 
  }
  
  if ((fd = open (file, O_CREAT|O_EXCL)) < 0)
    printf ("open (O_CREAT|O_EXCL) error for '%s', errno = %d\n", file, errno); 
  else 
  {
    printf ("open (O_CREAT|O_EXCL) OK for '%s'\n", file); 
    close (fd); 
  }

  DIR *dirent = NULL;
  if ((dirent = opendir (dir)) == 0)
    printf ("opendir error for '%s', errno = %d\n", dir, errno); 
  else 
  {
    printf ("opendir OK for '%s'\n", dir); 
    closedir (dirent); 
  }

  int n = 0; 
  if ((n = readlink (file, path, PATH_MAX-1)) < 0)
    printf ("readlink error for '%s', errno = %d\n", file, errno); 
  else 
  {
    path[n] = 0; 
    printf ("readlink OK for '%s', link=%s\n", file, path); 
  }

  if ((n = pathconf (file, _PC_NAME_MAX)) < 0)
    printf ("pathconf error for '%s', errno = %d\n", file, errno); 
  else 
    printf ("pathconf OK for '%s', limit=%d\n", file, n); 

#if 1
  char newfile[PATH_MAX] = { 0 }; 
  strcat (newfile, "./"); 
  strcat (newfile, file); 
  //chmod (
  if (execlp (newfile, "ls", "-l", NULL) < 0)
    printf ("execlp error for '%s', errno = %d\n", newfile, errno); 
  else 
    printf ("execlp OK for '%s'\n", newfile); 
#else
  if (truncate (file, 1024) < 0)
    printf ("truncate error for '%s', errno = %d\n", file, errno); 
  else 
    printf ("truncate OK for '%s', size=1024\n", file); 
#endif

#if 0
  if (remove (file) < 0)
    printf ("remove error for '%s', errno = %d\n", file, errno); 
  else 
    printf ("remove OK for '%s'\n", file); 
#endif 

#if 0
  char newfile[PATH_MAX] = { 0 }; 
  strcat (newfile, file); 
  strcat (newfile, ".bak"); 
  if (rename (file, newfile) < 0)
    printf ("rename error for '%s', errno = %d\n", file, errno); 
  else 
    printf ("rename OK for '%s'\n", file); 
#endif 

#if 0
  if ((fd = creat (file, 0755)) < 0)
    printf ("creat error for '%s', errno = %d\n", file, errno); 
  else 
  {
    printf ("creat OK for '%s'\n", file); 
    //unlink (file); 
    close (fd); 
  }
#endif 

  return 0; 
}
