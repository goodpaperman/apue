#include "../apue.h"
#include <utmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

struct utmp*  my_getutnam (char const* name)
{
  struct utmp *ptr = 0; 
  setutent (); 
  while ((ptr = getutent ()) != NULL)
  {
    struct in_addr addr = { 0 }; 
    addr.s_addr = ptr->ut_addr_v6[0]; 
    printf("type: %d, pid: %u, line: %s, utid: %.4s, user: %s, host: %s, exit: %d, sess: %d, time: %d, addr: %s\n", 
            ptr->ut_type, ptr->ut_pid, ptr->ut_line, ptr->ut_id, ptr->ut_user, ptr->ut_host, ptr->ut_exit.e_exit, 
            ptr->ut_session, ptr->ut_tv.tv_sec, inet_ntoa(addr));
    if (strcmp (name, ptr->ut_user) == 0)
      break; 
  }

  endutent (); 
  return (ptr); 
}

int main(int argc, char *argv[])
{
     struct utmp tmp;
     struct utmp *result;

     if (argc != 2) {
          fprintf(stderr, "Usage: %s username\n", argv[0]);
          exit(EXIT_FAILURE);
      }

      result = my_getutnam(argv[1]);
      if (result == NULL) {
          perror("getutnam");
          exit(EXIT_FAILURE);
      }

      tmp = *result; 
      printf ("find record!\n"); 
      exit(EXIT_SUCCESS);
}
