#include "../apue.h"

int main (int argc, char *argv[])
{
  int i; 
  char **ptr; 
  extern char **environ; 
  for (i=0; i<argc; ++ i)
    printf ("argv[%d]: %s\n", i, argv[i]); 

  printf ("\n"); 
  for (ptr = environ; *ptr != 0; ++ptr)
    printf ("%s\n", *ptr); 

  printf ("\n\n"); 
  exit (0); 
}
