#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  //int ret = 0; 
  if (argc < 4)
    err_sys ("Usage: fdopen_t path type1 type2"); 

  char const* path = argv[1]; 
  char const* type1  = argv[2]; 
  char const* type2 = argv[3]; 
  int flags = 0; 
  if (strchr (type1, 'r') != 0)
  {
    if (strchr (type1, '+') != 0)
      flags |= O_RDWR; 
    else 
      flags |= O_RDONLY;
  }
  else if (strchr (type1, 'w') != 0)
  {
    flags |= O_TRUNC; 
    if (strchr (type1, '+') != 0)
      flags |= O_RDWR; 
    else 
      flags |= O_WRONLY;
  }
  else if (strchr (type1, 'a') != 0)
  {
    flags |= O_APPEND; 
    if (strchr (type1, '+') != 0)
      flags |= O_RDWR; 
    else 
      flags |= O_WRONLY;
  }

  int fd = open (path, flags, 0777);  
  if (fd == 0)
    err_sys ("fopen failed"); 

  //printf ("open with flags %d (type %s) OK\n", flags, type1); 
  printf ("(%d) open type %s, type %s ", getpid (), type1, type2);
  FILE* fp = fdopen (fd, type2); 
  if (fp == 0)
  {
    err_sys ("fdopen failed"); 
    close (fd); 
  }

  //printf ("fdopen with type %s OK\n", type2); 
  printf ("OK\n"); 

#ifdef TEST_TRUNC
  fseek (fp, 0, SEEK_END); 
  printf ("size  = %ld\n", ftell (fp)); 
  fprintf (fp, "abcdefghijklmnopqrstuvwxyz"); 
  fseek (fp, 0, SEEK_END); 
  printf ("new size  = %ld\n", ftell (fp)); 
#endif 

#ifdef TEST_APPEND
  int write = fprintf (fp, "%d ", getpid ()); 
  fflush (fp); 

  char buf[1024] = { 0 }; 
  printf ("write %d, curr %ld\n", write, ftell (fp)); 
  fseek (fp, 0, SEEK_SET); 
  fread (buf, 1024, 1, fp); 
  printf ("read: %s\n", buf); 
#endif

  fclose (fp); 
  //sleep (1); 
  return 0; 
}
