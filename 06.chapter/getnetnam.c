#include <netdb.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"

int
main(int argc, char *argv[])
{
     struct netent *result;
     if (argc != 2) {
          fprintf(stderr, "Usage: %s hostname\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = getnetbyname(argv[1]);
      if (result == NULL) {
          perror("getnetbyname");
          exit(EXIT_FAILURE);
      }

      struct in_addr addr; 
      addr.s_addr = result->n_net; 
      printf("Name: %s; type: %d; addr: %s\n", result->n_name, result->n_addrtype, inet_ntoa(addr));

      /*
      printf ("-------alias-------------\n"); 
      char **ptr = result->n_aliases; 
      while (ptr && *ptr)
      {
        printf ("  %s\n", *ptr); 
        ++ ptr; 
      }
      */
      exit(EXIT_SUCCESS);
}
