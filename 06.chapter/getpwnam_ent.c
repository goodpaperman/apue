#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"


struct passwd* 
my_getpwnam (char const* name)
{
  struct passwd *ptr = 0; 
  setpwent (); 
  while ((ptr = getpwent ()) != NULL)
  {
    if (strcmp (name, ptr->pw_name) == 0)
      break; 
  }

  endpwent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct passwd pwd;
     struct passwd *result;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s username\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_getpwnam(argv[1]);
      if (result == NULL) {
          perror("getpwnam");
          exit(EXIT_FAILURE);
      }

      pwd = *result; 
      printf("Name: [%p] %s; UID: %ld\n", pwd.pw_gecos, pwd.pw_gecos, (long) pwd.pw_uid);
      exit(EXIT_SUCCESS);
}
