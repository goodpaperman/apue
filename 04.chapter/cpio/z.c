#include "../apue.h"
#include <errno.h>
//#include <pwd.h>
//#include <linux/limits.h>
//#include <dirent.h> 
#include <time.h> 
//#include <libgen.h> 

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
  printf ("argv[0] = %s\n", argv[0]); 
  sleep (2);
  print_ftime (argv[0]);
  return 0; 
}
