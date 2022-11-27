#include "../apue.h"
#include <grp.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


struct group* 
my_getgrnam (char const* name)
{
  struct group *ptr = 0; 
  setgrent (); 
  while ((ptr = getgrent ()) != NULL)
  {
    if (strcmp (name, ptr->gr_name) == 0)
      break; 

    printf ("%s\n", ptr->gr_name); 
  }

  endgrent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct group grp;
     struct group *result;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s group\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_getgrnam(argv[1]);
      if (result == NULL) {
          perror("getgrnam");
          exit(EXIT_FAILURE);
      }

      grp = *result; 
      printf("Name: %s; GID: %d\n", grp.gr_name, grp.gr_gid);
      for (int i=0; grp.gr_mem[i] != 0; ++i)
        printf ("  %s\n", grp.gr_mem[i]); 
      exit(EXIT_SUCCESS);
}
