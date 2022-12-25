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
     struct servent *result;
     if (argc != 3) {
          fprintf(stderr, "Usage: %s service protocol\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = getservbyname(argv[1], argv[2]);
      if (result == NULL) {
          perror("getservbyname");
          exit(EXIT_FAILURE);
      }

      printf("Name: %s; port: %d, proto: %s\n", result->s_name, ntohs(result->s_port), result->s_proto);

      printf ("-------alias-------------\n"); 
      char **ptr = result->s_aliases; 
      while (ptr && *ptr)
      {
        printf ("  %s\n", *ptr); 
        ++ ptr; 
      }
      exit(EXIT_SUCCESS);
}
