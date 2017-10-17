#include <netdb.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"


struct servent* 
my_getservnam (char const* name, char const* proto)
{
  struct servent *ptr = 0; 
  setservent (1); 
  while ((ptr = getservent ()) != NULL)
  {
    printf ("searching %s, %s\n", ptr->s_name, ptr->s_proto); 
    if (strcmp (name, ptr->s_name) == 0 
    &&  strcmp (proto, ptr->s_proto) == 0)
      break; 
  }

  endservent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct servent *result;

     if (argc != 3) {
          fprintf(stderr, "Usage: %s service protocol\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_getservnam(argv[1], argv[2]);
      if (result == NULL) {
          perror("getservnam");
          exit(EXIT_FAILURE);
      }

      printf("Name: %s; port: %d, proto: %s\n", result->s_name, result->s_port, result->s_proto);
      printf ("-------alias-------------\n"); 
      char **ptr = result->s_aliases; 
      while (ptr && *ptr)
      {
        printf ("  %s\n", *ptr); 
        ++ ptr; 
      }

      exit(EXIT_SUCCESS);
}
