#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include "../../apue.h"

#define MAX_BUFSIZE 512

void read_some (int connfd)
{
  int ret = 0; 
  char buf[MAX_BUFSIZE+1] = { 0 }; 
  // to avoid quit read nothing
  usleep (100000); 
  do
  {
    ret = recv (connfd, buf, sizeof(buf), MSG_DONTWAIT); 
    if (ret <= 0)
    {
        if (errno == EAGAIN)
            break; 

        printf ("recv failed, errno %d\n", errno); 
        break; 
    }

    buf[ret] = 0; 
    printf ("%s", buf); 
  }while (1); 
}

int main (int argc, char *argv[])
{
    int ret = 0; 
    unsigned short port = 10028; 
    struct sockaddr_in addr; 
    addr.sin_family = AF_INET; 
    addr.sin_port = htons (port); 
    addr.sin_addr.s_addr = htonl (INADDR_ANY); 

    int connfd = socket (PF_INET, SOCK_STREAM, 0); 
    if (connfd < 0)
        err_sys ("create socket"); 

    socklen_t addrlen = sizeof (struct sockaddr_in); 
    ret = connect (connfd, (struct sockaddr *)&addr, addrlen); 
    if (ret < 0)
        err_sys ("connect"); 

    read_some (connfd); 
    int n = 0, quit = 0; 
    char buf[MAX_BUFSIZE+1] = { 0 }; 
    while (!quit)
    {
      //printf (">"); 
      if (fgets (buf, MAX_BUFSIZE, stdin) == NULL)
      {
          printf ("getting user input failed\n"); 
          break; 
      }

      // to skip terminal \n
      if (strncmp (buf, "exit", 4) == 0)
      {
          quit = 1; 
          //printf ("%d: %s", quit, buf); 
      }
      else if (strncmp (buf, "ctlc", 4) == 0)
      {
          buf[0]= 03;  // 'C' - 'A' + 1
          buf[1] = 0; 
      }
      else if (strncmp (buf, "ctld", 4) == 0)
      {
          buf[0]= 04;  // 'D' - 'A' + 1
          buf[1] = 0; 
      }
      else if (strncmp (buf, "ctlz", 4) == 0)
      {
          buf[0]= 032;  // 'Z' - 'A' + 1
          buf[1] = 0; 
      }
      else if (strncmp (buf, "ctlq", 4) == 0)
      {
          buf[0]= 0134 - 0100;  // '\\' - 'A' + 1
          buf[1] = 0; 
          //printf ("%o", buf[0]); 
      }
      
      while ((n = strlen (buf)) > 0 && buf[n-1] == '\n')
          buf[n-1] = 0; 

      ret = send (connfd, buf, n, 0);
      if (ret < n)
      {
          printf ("send failed, %d != %d\n", ret, n); 
          break; 
      }

      read_some (connfd); 

    }
 
    printf ("connection break...\n"); 
    close (connfd); 
    return 0; 
}
