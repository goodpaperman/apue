#include "../apue.h" 
#include <dirent.h> 

int 
main (int argc, char *argv[]) 
{
  DIR *dp = 0; 
  struct dirent *dirp = 0; 
  if (argc != 2)
    err_quit ("usage: ls directory_name"); 

  if ((dp = opendir (argv[1])) == 0)
    err_sys ("can't open %s", argv[1]); 
  
  while ((dirp = readdir (dp)) != 0)
    printf ("%s\n", dirp->d_name); 

  closedir (dp); 

  int len = 0; 
  char *ptr = path_alloc (&len); 
  printf ("path length = %d\n", len); 
  free (ptr); 
  long openmax = open_max (); 
  printf ("open max = %ld\n", openmax); 
  exit (0); 
}
