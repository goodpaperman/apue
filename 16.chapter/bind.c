#include "../apue.h" 
#include <sys/socket.h> 
#include <netinet/in.h> // for INADDR_ANY
#include <arpa/inet.h>  // for inet_ntop
#include <errno.h> 

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        printf ("Usage: bind port\n"); 
        return -1; 
    }

    unsigned short port = atoi (argv[1]); 
    int fd = -1; 

    do
    {
        fd = socket (AF_INET, SOCK_STREAM, 0); 
        if (fd == -1) {
            printf ("socket call failed, errno %d\n", errno); 
            break; 
        }

        struct sockaddr_in addr = { 0 }; 
        addr.sin_addr.s_addr = INADDR_ANY; 
        addr.sin_port = htons (port); 
        int ret = bind (fd, (struct sockaddr *)&addr, sizeof (addr)); 
        if (ret == -1) {
            printf ("bind call failed, errno %d\n", errno); 
            break; 
        }

        printf ("bind to %d ok\n", port); 

        socklen_t len = sizeof (addr); 
        ret = getsockname (fd, (struct sockaddr *)&addr, &len); 
        if (ret == -1) {
            printf ("getsockname failed, errno %d\n", errno); 
            break; 
        }

        char buf[INET_ADDRSTRLEN] = { 0 }; 
        char const* ptr = inet_ntop (AF_INET, &addr.sin_addr, buf, INET_ADDRSTRLEN); 
        printf ("local addr %d: %s\n", len, ptr ? ptr : "unknown"); 

        len = sizeof (addr); 
        ret = getpeername (fd, (struct sockaddr *)&addr, &len); 
        if (ret == -1) {
            printf ("getpeername failed, errno %d\n", errno); 
            break; 
        }

        ptr = inet_ntop (AF_INET, &addr.sin_addr, buf, INET_ADDRSTRLEN); 
        printf ("remote addr %d: %s\n", len, ptr ? ptr : "unknown"); 

        sleep (10); 
    } while (0); 

    if (fd != -1)
        close (fd); 

    return 0; 
}
