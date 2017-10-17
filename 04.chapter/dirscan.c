#include <stdio.h> 
#include <stdlib.h> 
#include <dirent.h> 
#include <string.h> 

int main (void)
{
  struct dirent **namelist; 
  int n = scandir (".", &namelist, 0, alphasort); 
  if (n < 0)
    perror ("scandir"); 
  else 
  {
    while (n--) {
      printf ("%s\n", namelist[n]->d_name); 
      free (namelist[n]); 
    }

    free (namelist); 
  }

  return 0; 
}
