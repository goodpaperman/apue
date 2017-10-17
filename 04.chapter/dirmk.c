#include "../apue.h"
#include <errno.h>
//#include <pwd.h>
//#include <dirent.h> 
#include <time.h> 
//#include <libgen.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <unistd.h> 


int main (int argc, char *argv[])
{
  char dir[27] = { 0 }; 
  int total = 0; 
  srand (time(0)); 
  while (total <= PATH_MAX)
  {
    for (int n = 0; n < 26; ++ n)
      dir[n] = "abcdefghijklmnopqrstuvwxyz"[rand () % 26]; 

    if (mkdir (dir, 0764) == 0)
    {
      printf ("mkdir %s OK\n", dir); 
      if (chdir (dir) == -1)
      {
        printf ("chdir %s failed\n", dir); 
        break; 
      }
    }
    else
    {
      printf ("mkdir %s failed\n", dir); 
      break; 
    }

    total += strlen (dir); 
  }

  printf ("total length = %d\n", total); 

  char path[PATH_MAX+1] = { 0 }; 
  printf ("getwd = %s\n", getwd(path)); 

  int size = PATH_MAX; 
  char *cwd = 0, *ptr = malloc (size); 
  while (1) {
    cwd = getcwd (ptr, size); 
    if (cwd == 0)
    {
      if (errno == ERANGE)
      {
        size *= 2; 
        ptr = realloc (ptr, size); 
        printf ("try size %d, ptr = 0x%x\n", size, (int)ptr); 
        if (ptr == 0)
          break; 
        else 
          continue; 
      }
      else 
      {
        printf ("fatal error %d\n", errno); 
        break; 
      }
    }
    else 
    {
      printf ("getcwd OK, cwd = %s\n", cwd); 
      break; 
    }
  } 

  printf ("getcwd = %s\n", cwd); 
  free (cwd); 

  cwd = get_current_dir_name (); 
  printf ("get_current_dir_name = %s\n", cwd); 
  free (cwd); 
  return total; 
}
