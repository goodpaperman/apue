#include "../apue.h"
#include <limits.h> 
#include <unistd.h> 

int main (int argc, char *argv[])
{
  if (chdir (argv[1]) < 0)
    err_sys ("chdir failed"); 

  printf ("chdir to %s succeeded\n", argv[1]); 
  char path[PATH_MAX+1] = { 0 }; 
  printf ("getwd = %s\n", getwd(path)); 
  char *cwd = getcwd (path, PATH_MAX); 
  printf ("getcwd = %s\n", cwd); 
  cwd = get_current_dir_name (); 
  printf ("get_current_dir_name = %s\n", cwd); 
  free (cwd); 
  exit (0); 
}
