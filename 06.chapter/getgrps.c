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
     int result;
     gid_t  *groups;
     struct group *grp; 
     size_t grpmax;

      grpmax = sysconf(_SC_NGROUPS_MAX);
      if (grpmax == -1)          /* Value was indeterminate */
          grpmax = 16;        /* Should be more than enough */

      printf ("grpmax = %d\n", grpmax); 
      grpmax = getgroups (0, 0); 
      printf ("grpmax by getgroups = %d\n", grpmax); 
      groups = malloc(grpmax);
      if (groups == NULL) {
          perror("malloc");
          exit(EXIT_FAILURE);
      }

      result = getgroups (grpmax, groups); 
      if (result == -1) {
          perror("getgroups");
          exit(EXIT_FAILURE);
      }

      printf("group count: %d\n", result);
      for (int i=0; i < result; ++i)
      {
        grp = getgrgid (groups[i]); 
        printf ("  [%d] %s\n", groups[i], grp->gr_name); 
      }

      exit(EXIT_SUCCESS);
}
