#include "../apue.h"
#include <errno.h>
#include <pwd.h>
#include <linux/limits.h>
#include <dirent.h> 
#include <time.h> 
#include <libgen.h> 
#include <utime.h> 
#include <sys/stat.h> 

void print_ftime (char const* file)
{
  struct tm* ptm = 0; 
  struct stat statbuf = { 0 }; 
  if (lstat (file, &statbuf) < 0)
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

void print_dtime (int fd)
{
  struct tm* ptm = 0; 
  struct stat statbuf = { 0 }; 
  if (fstat (fd, &statbuf) < 0)
    printf ("stat error for '%d', errno = %d\n", fd, errno); 
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
  char const* file = 0, *file2 = 0; 
  
#ifdef SEE_BASE
  file = dirname((char *)from); 
  file2 = dirname((char *)to); 
#else 
  file = from; 
  file2 = to; 
#endif 

  printf ("file = %s, file2 = %s\n", file, file2); 
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

#if 0
  char newfile[PATH_MAX] = { 0 }; 
  strcat (newfile, "./"); 
  strcat (newfile, from); 
  if (execlp (newfile, newfile, NULL) < 0)
    printf ("execlp error for '%s', errno = %d\n", newfile, errno); 
  else 
    printf ("execlp OK for '%s'\n", newfile); 
#endif 

  struct passwd *pwd = 0; 
  pwd = getpwnam ("yunhai"); 
  int uid = pwd->pw_uid; 
  printf ("uid of 'yunhai' is %d\n", uid); 
  if (chown (from, uid, uid) < 0)
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
    printf ("open readonly OK for '%s'\n", from); 

  sleep (2); 
  print_ftime (file); 

  char buf[32] = { 0 }; 
  if (read (fd, buf, 30) == -1)
    printf ("read for '%s' failed, errno = %d\n", from, errno); 
  else 
    printf ("read OK for '%s'\n", from); 

  close (fd); 
  sleep (2); 
  print_ftime (file); 

  if ((fd = open (from, O_WRONLY | O_TRUNC)) < 0)
    printf ("open error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("open truncate OK for '%s'\n", from); 
  
  sleep (2); 
  print_ftime (file); 

  if (write (fd, buf, 30) <= 0)
    printf ("write for '%s' failed, errno = %d\n", from, errno); 
  else 
    printf ("write OK for '%s'\n", from); 

  close (fd); 
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

  DIR *dirent = NULL;
  if ((dirent = opendir (from)) == 0)
    printf ("opendir error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("opendir OK for '%s'\n", from); 

  sleep (2); 
  print_ftime (file); 

  struct dirent* dir = NULL; 
  if ((dir = readdir (dirent)) == 0)
    printf ("readdir error, errno = %d\n", errno); 
  else 
    printf ("readdir OK, dname = %s\n", dir->d_name); 

  closedir (dirent); 
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

  if (rename (from, to) < 0)
    printf ("rename error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("rename OK for '%s'\n", from); 

  sleep (2); 
#ifdef SEE_BASE
  print_ftime (file); 
#else
  print_ftime (to); 
#endif 
  unlink (to); 

  if ((fd = creat (from, 0755)) < 0)
    printf ("creat error for '%s', errno = %d\n", from, errno); 
  else 
  {
    printf ("creat OK for '%s'\n", from); 
    close (fd); 
  }

  sleep (2); 
  print_ftime (file); 

  if (truncate (from, 1024) < 0)
    printf ("truncate error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("truncate OK for '%s', size=1024\n", from); 

  sleep (2); 
  print_ftime (file); 

  if (utime (from, 0) < 0)
    printf ("utime error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("utime OK for '%s', timbuf = NULL\n", from); 

  sleep (2);
  print_ftime (file); 

  struct utimbuf tbuf = { 0 }; 
  tbuf.actime = time (0) - 10; 
  tbuf.modtime = time (0) + 10; 
  if (utime (from, &tbuf) < 0)
    printf ("utime error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("utime OK for '%s', actime = %ld, modtime = %ld\n", from, tbuf.actime, tbuf.modtime); 

  sleep (2);
  print_ftime (file); 

  if (symlink (from, to) < 0)
    printf ("symlink error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("symlink OK for '%s', link=%s\n", from, to); 

  sleep (2); 
  print_ftime (file); 
  print_ftime (file2); 

  struct timespec times[2] = {{ 0 }}, *ptime = &times[0]; 
  times[0].tv_sec = time(0) - 10; 
  times[1].tv_sec = time(0) + 10; 
  if (utimensat (AT_FDCWD, to, ptime, AT_SYMLINK_NOFOLLOW) < 0)
    printf ("utimensat error for '%s', errno = %d\n", to, errno); 
  else 
    printf ("utimensat OK for '%s', actime = %ld, modtime = %ld\n", to, tbuf.actime, tbuf.modtime); 

  sleep (2);
  print_ftime (file); 
  print_ftime (file2); 

  int n = 0; 
  char path[PATH_MAX] = { 0 }; 
  if ((n = readlink (to, path, PATH_MAX-1)) < 0)
    printf ("readlink error for '%s', errno = %d\n", to, errno); 
  else 
  {
    path[n] = 0; 
    printf ("readlink OK for '%s', link=%s\n", to, path); 
  }

  sleep (2);
  print_ftime (file); 
  print_ftime (file2); 
  unlink (to); 
  unlink (from); 

  if (mkfifo (from, 0755) < 0)
    printf ("mkfifo error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("mkfifo OK for '%s'\n", from); 

  sleep (2);
  print_ftime (file); 
  unlink (from); 

  int fds[2] = { 0 }; 
  if (pipe (fds) < 0)
    printf ("pipe failed, errno = %d\n", errno); 
  else 
    printf ("pipe OK, fd0 = %d, fd1 = %d\n", fds[0], fds[1]); 

  sleep (2);
  print_dtime (fds[0]); 
  print_dtime (fds[1]); 


#if 0
  if ((n = pathconf (from, _PC_NAME_MAX)) < 0)
    printf ("pathconf error for '%s', errno = %d\n", from, errno); 
  else 
    printf ("pathconf OK for '%s', limit=%d\n", from, n); 
#endif 

  return 0; 
}
