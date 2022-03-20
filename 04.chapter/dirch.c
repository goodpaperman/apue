#include "../apue.h"
#include <limits.h> 
#include <unistd.h> 

void ch_dir(char const* dir)
{
  if (chdir (dir) < 0)
    err_sys ("chdir failed"); 

  printf ("chdir to %s succeeded\n", dir); 
  char path[PATH_MAX+1] = { 0 }; 
  printf ("getwd = %s\n", getwd(path)); 
  char *cwd = getcwd (path, PATH_MAX); 
  printf ("getcwd = %s\n", cwd); 
  cwd = get_current_dir_name (); 
  printf ("get_current_dir_name = %s\n", cwd); 
  free (cwd); 
}

int main (int argc, char *argv[])
{
  char dir[PATH_MAX] = { 0 }, *dir_name = NULL; 
  if (argc <= 2)
    strcpy(dir, argv[1]); 
  else if (argc <= 3)
  {
    strcpy(dir, argv[1]); 
    dir_name = argv[2]; 
  }
  else 
    err_quit ("Usage: dirch dir [dirname]", -1); 

  ch_dir(dir); 
  while (dir_name /*&& strlen(dir) < PATH_MAX*/)
  {
    //strcat (dir, "/"); 
    //strcat (dir, dir_name); 
    ch_dir(dir_name);
  }

  return 0; 
}
