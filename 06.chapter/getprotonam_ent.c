#include <netdb.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"


struct protoent* 
my_getprotonam (char const* name)
{
  struct protoent *ptr = 0; 
  setprotoent (1); 
  while ((ptr = getprotoent ()) != NULL)
  {
    printf ("searching %s\n", ptr->p_name); 
    if (strcmp (name, ptr->p_name) == 0)
      break; 
  }

  endprotoent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct protoent *result;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s protocol\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_getprotonam(argv[1]);
      if (result == NULL) {
          perror("getprotonam");
          exit(EXIT_FAILURE);
      }

      printf("Name: %s; type: %d\n", result->p_name, result->p_proto);
      printf ("-------alias-------------\n"); 
      char **ptr = result->p_aliases; 
      while (ptr && *ptr)
      {
        printf ("  %s\n", *ptr); 
        ++ ptr; 
      }

      exit(EXIT_SUCCESS);
}
