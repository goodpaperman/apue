#include "../apue.h"
#include <errno.h>
#include <pwd.h>
#include <linux/limits.h>
#include <dirent.h> 
#include <time.h> 
#include <libgen.h> 

void print_ftime (char const* file)
{
  struct tm* ptm = 0; 
  struct stat statbuf = { 0 }; 
  if (stat (file, &statbuf) < 0)
    printf ("stat error for '%s', errno = %d\n", file, errno); 
  else 
  {
    char stra[64], strm[64], strc[64]; 
    ptm = localtime (&statbuf.st_atime); 
    sprintf (stra, "%d-%d-%d %d:%d:%d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec); 
    ptm = localtime (&statbuf.st_mtime); 
    sprintf (strm, "%d-%d-%d %d:%d:%d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec); 
    ptm = localtime (&statbuf.st_ctime); 
    sprintf (strc, "%d-%d-%d %d:%d:%d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec); 
    printf ("atime: %s, mtime: %s, ctime: %s\n", stra, strm, strc); 
  }
}


int main (int argc, char *argv[])
{
  if (argc != 3)
    err_quit ("usage: ftimes <from> <to>"); 

  int fd = 0; 
  char const* from = argv[1]; 
  char const* to = argv[2]; 
  char const* file = 0; 
  
#ifdef SEE_BASE
  file = dirname((char *)from); 
#else 
  file = from; 
#endif 

  printf ("file = %s\n", file); 
  print_ftime (file); 
  if (access (from, F_OK) < 0)
    printf ("access error for '%s', errno = %d\n", from, errno);
  else 
    printf ("access OK for '%s'\n", from); 

  sleep (2); 
  print_ftime (file); 
  if (chmod (from, 0777) < 0)
    printf ("chmod error for '%s', errno = %d\n", from, errno); 
  else 
  {
    printf ("chmod OK for '%s'\n", from); 
    // restore original permission
    //chmod (from, 0644); 
  }
  
  sleep (2); 
  print_ftime (file); 
  struct passwd *pwd = 0; 
  pwd = getpwnam ("yunhai"); 
  int uid = pwd->pw_uid; 
  printf ("uid of 'yunhai' is %d\n", uid); 
  if ((fd = chown (from, uid, uid)) < 0)
    printf ("chown error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("chown OK for '%s'\n", from); 

  sleep (2); 
  print_ftime (file); 
  if (link (from, to) < 0)
    printf ("link error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("link OK for '%s', new from '%s'\n", from, to); 
  
  sleep (2); 
  print_ftime (file); 
  if (unlink (to) < 0)
    printf ("unlink error for '%s', errno = %d\n", to, errno); 
  else 
    printf ("unlink OK for '%s'\n", to); 
  
  sleep (2); 
  print_ftime (file); 
  if ((fd = open (from, O_RDONLY)) < 0)
    printf ("open error for '%s', errno = %d\n", from, errno); 
  else 
  {
    printf ("open readonly OK for '%s'\n", from); 
    char buf[32] = { 0 }; 
    read (fd, buf, 30); 
    close (fd); 
  }
  
  sleep (2); 
  print_ftime (file); 
  if ((fd = open (from, O_WRONLY | O_TRUNC)) < 0)
    printf ("open error for '%s', errno = %d\n", from, errno); 
  else 
  {
    printf ("open truncate OK for '%s'\n", from); 
    close (fd); 
  }
  
  sleep (2); 
  print_ftime (file); 
  if (remove (from) < 0)
    printf ("remove error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("remove OK for '%s'\n", from); 

  sleep (2); 
  print_ftime (file); 
  if (mkdir (from, 0777) < 0)
    printf ("mkdir error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("mkdir OK for '%s'\n", from); 

  sleep (2); 
  print_ftime (file); 
#if 0
  if (rmdir (from) < 0)
    printf ("rmdir error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("rmdir OK for '%s'\n", from); 
#else
  if (remove (from) < 0)
    printf ("remove error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("remove dir OK for '%s'\n", from); 
#endif 

  sleep (2); 
  print_ftime (file); 
  if ((fd = open (from, O_RDWR | O_CREAT, 0777)) < 0)
    printf ("open error for '%s', errno = %d\n", from, errno); 
  else 
  {
    printf ("open create OK for '%s'\n", from); 
    close (fd); 
  }
  
  sleep (2); 
  print_ftime (file); 
#if 0
  DIR *dirent = NULL;
  if ((dirent = opendir (to)) == 0)
    printf ("opendir error for '%s', errno = %d\n", to, errno); 
  else 
  {
    printf ("opendir OK for '%s'\n", to); 
    closedir (dirent); 
  }

  int n = 0; 
  if ((n = readlink (from, path, PATH_MAX-1)) < 0)
    printf ("readlink error for '%s', errno = %d\n", from, errno); 
  else 
  {
    path[n] = 0; 
    printf ("readlink OK for '%s', link=%s\n", from, path); 
  }

  if ((n = pathconf (from, _PC_NAME_MAX)) < 0)
    printf ("pathconf error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("pathconf OK for '%s', limit=%d\n", from, n); 
#endif 

#if 0
  char newfile[PATH_MAX] = { 0 }; 
  strcat (newfile, "./"); 
  strcat (newfile, from); 
  //chmod (
  if (execlp (newfile, "ls", "-l", NULL) < 0)
    printf ("execlp error for '%s', errno = %d\n", newfile, errno); 
  else 
    printf ("execlp OK for '%s'\n", newfile); 
#elif 0
  if (truncate (from, 1024) < 0)
    printf ("truncate error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("truncate OK for '%s', size=1024\n", from); 
#endif

#if 0
  char newfile[PATH_MAX] = { 0 }; 
  strcat (newfile, from); 
  strcat (newfile, ".bak"); 
  if (rename (from, newfile) < 0)
    printf ("rename error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("rename OK for '%s'\n", from); 
#endif 

#if 0
  if ((fd = creat (from, 0755)) < 0)
    printf ("creat error for '%s', errno = %d\n", from, errno); 
  else 
  {
    printf ("creat OK for '%s'\n", from); 
    //unlink (from); 
    close (fd); 
  }
#endif 

  return 0; 
}
