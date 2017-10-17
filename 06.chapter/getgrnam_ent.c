#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"


struct group* 
my_getgrnam (char const* name)
{
  struct group *ptr = 0; 
  setgrent (); 
  while ((ptr = getgrent ()) != NULL)
  {
    if (strcmp (name, ptr->gr_name) == 0)
      break; 
  }

  endgrent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct group pwd;
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

      pwd = *result; 
      printf("Name: %s; GID: %d\n", pwd.gr_name, pwd.gr_gid);
      for (int i=0; pwd.gr_mem[i] != 0; ++i)
        printf ("  %s\n", pwd.gr_mem[i]); 
      exit(EXIT_SUCCESS);
}
