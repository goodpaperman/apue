//#include <pwd.h>
#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"

int
main(int argc, char *argv[])
{
     struct spwd pwd;
     struct spwd *result;
     char *buf;
     size_t bufsize;
     int s;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s username\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
      if (bufsize == -1)          /* Value was indeterminate */
          bufsize = 16384;        /* Should be more than enough */

      buf = malloc(bufsize);
      if (buf == NULL) {
          perror("malloc");
          exit(EXIT_FAILURE);
      }

      s = getspnam_r(argv[1], &pwd, buf, bufsize, &result);
      if (result == NULL) {
          if (s == 0)
              printf("Not found\n");
          else {
              errno = s;
              perror("getspnam_r");
          }
          exit(EXIT_FAILURE);
      }

      printf("Name: [%p] %s; Pwd [%p]: %s\n", pwd.sp_namp, pwd.sp_namp, pwd.sp_pwdp, pwd.sp_pwdp);
      char *p = buf; 
      while (p && strlen (p) > 0)
      {
        printf("[%p] %s\n", p, p); 
        p += strlen (p) + 1; 
      }
      exit(EXIT_SUCCESS);
}
