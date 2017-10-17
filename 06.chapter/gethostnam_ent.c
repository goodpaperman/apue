#include <netdb.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"


struct hostent* 
my_gethostnam (char const* name)
{
  struct hostent *ptr = 0; 
  sethostent (1); 
  while ((ptr = gethostent ()) != NULL)
  {
    printf ("searching %s\n", ptr->h_name); 
    if (strcmp (name, ptr->h_name) == 0)
      break; 
  }

  endhostent (); 
  return (ptr); 
}

int
main(int argc, char *argv[])
{
     struct hostent *result;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s hostname\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_gethostnam(argv[1]);
      if (result == NULL) {
          perror("gethostnam");
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
      exit(EXIT_SUCCESS);
}
