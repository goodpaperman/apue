#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../apue.h"

int
main(int argc, char *argv[])
{
     int result;
     struct passwd *pwd; 

     if (argc != 2) {
          fprintf(stderr, "Usage: %s username\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      pwd = getpwnam (argv[1]); 
      if (pwd == NULL)
      {
          perror ("getpwnam"); 
          exit(EXIT_FAILURE); 
      }

      printf ("pwd name %s, uid %d, gid %d\n", pwd->pw_name, pwd->pw_uid, pwd->pw_gid); 
      result = initgroups(pwd->pw_name, pwd->pw_gid); 
      if (result == -1) {
          perror("initgroups");
          exit(EXIT_FAILURE);
      }

      printf ("setgroups OK, username : %s\n", pwd->pw_name); 
      int num = getgroups (0, NULL); 
      gid_t *grps = malloc(sizeof(gid_t) *num); 
      printf ("group count = %d\n", num); 
      num = getgroups (num, grps); 
      if (num == -1)
      {
        perror ("getgroups"); 
        exit (EXIT_FAILURE); 
      }

      struct group *grp; 
      for (int i=0; i<num; ++ i)
      {
        grp = getgrgid (grps[i]); 
        printf ("  [%d] %s\n", grp->gr_gid, grp->gr_name); 
      }

      free (grps); 
      exit(EXIT_SUCCESS);
}
