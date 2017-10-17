#include <sys/types.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"

int
main(int argc, char *argv[])
{
     struct group pwd;
     struct group *result;
     char *buf;
     size_t bufsize;
     int s;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s groupname\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
      if (bufsize == -1)          /* Value was indeterminate */
          bufsize = 16384;        /* Should be more than enough */

      buf = malloc(bufsize);
      if (buf == NULL) {
          perror("malloc");
          exit(EXIT_FAILURE);
      }

      s = getgrnam_r(argv[1], &pwd, buf, bufsize, &result);
      if (result == NULL) {
          if (s == 0)
              printf("Not found\n");
          else {
              errno = s;
              perror("getgrnam_r");
          }
          exit(EXIT_FAILURE);
      }

      printf("Name: [%p] %s; GID: %ld\n", pwd.gr_name, pwd.gr_name, (long) pwd.gr_gid);
      for (int i=0; pwd.gr_mem[i] != 0; ++i)
        printf ("  [%p] %s\n", pwd.gr_mem[i], pwd.gr_mem[i]); 

      char *p = buf; 
      while (p && strlen (p) > 0)
      {
        printf("[%p] %s\n", p, p); 
        p += strlen (p) + 1; 
      }
      exit(EXIT_SUCCESS);
}
