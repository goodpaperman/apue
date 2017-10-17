#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"


struct spwd* 
my_getspnam (char const* name)
{
  struct spwd *ptr = 0; 
  setspent (); 
  while ((ptr = getpwent ()) != NULL)
  {
    if (strcmp (name, ptr->sp_namp) == 0)
      break; 
  }

  endpwent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct spwd pwd;
     struct spwd *result;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s username\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_getspnam(argv[1]);
      if (result == NULL) {
          perror("my_getspnam");
          exit(EXIT_FAILURE);
      }

      pwd = *result; 
      printf("Name: %s; Pwd: %s\n", pwd.sp_namp, pwd.sp_pwdp);
      exit(EXIT_SUCCESS);
}
