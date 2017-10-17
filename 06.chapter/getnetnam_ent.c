#include <netdb.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"


struct netent* 
my_getnetnam (char const* name)
{
  struct netent *ptr = 0; 
  setnetent (1); 
  while ((ptr = getnetent ()) != NULL)
  {
    printf ("searching %s\n", ptr->n_name); 
    if (strcmp (name, ptr->n_name) == 0)
      break; 
  }

  endnetent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct netent *result;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s hostname\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_getnetnam(argv[1]);
      if (result == NULL) {
          perror("getnetnam");
          exit(EXIT_FAILURE);
      }

      struct in_addr addr; 
      addr.s_addr = result->n_net; 
      printf("Name: %s; type: %d; addr: %s\n", result->n_name, result->n_addrtype, inet_ntoa(addr));

      exit(EXIT_SUCCESS);
}
