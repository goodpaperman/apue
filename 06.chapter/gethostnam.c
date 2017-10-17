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
     struct hostent *result;
     if (argc != 2) {
          fprintf(stderr, "Usage: %s hostname\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = gethostbyname(argv[1]);
      if (result == NULL) {
          perror("gethostbyname");
          exit(EXIT_FAILURE);
      }

      printf("Name: %s; type: %d\n", result->h_name, result->h_addrtype);

      int i = 0; 
      char **p = result->h_addr_list; 
      while (p && p[i])
      {
        printf("  %s\n", inet_ntoa(*(struct in_addr*)p[i])); 
        i++; 
      }

      /*
      printf ("-------alias-------------\n"); 
      char **ptr = result->h_aliases; 
      while (ptr && *ptr)
      {
        printf ("  %s\n", *ptr); 
        ++ ptr; 
      }
      */
      exit(EXIT_SUCCESS);
}
