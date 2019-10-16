#include "../apue.h" 
#include <sys/socket.h> 
#include <netinet/in.h> // for INADDR_ANY
#include <arpa/inet.h>  // for inet_ntop
#include <errno.h> 

void dump_addr (int fd)
{
    struct sockaddr_in addr = { 0 }; 
    socklen_t len = sizeof (addr); 
    int ret = getsockname (fd, (struct sockaddr *)&addr, &len); 
    if (ret == -1) {
        printf ("getsockname failed, errno %d\n", errno); 
        return; 
    }

    char buf[INET_ADDRSTRLEN] = { 0 }; 
    char const *ptr = inet_ntop (AF_INET, &addr.sin_addr, buf, INET_ADDRSTRLEN); 
    printf ("local addr %d: %s\n", len, ptr ? ptr : "unknown"); 

    len = sizeof (addr); 
    ret = getpeername (fd, (struct sockaddr *)&addr, &len); 
    if (ret == -1) {
        printf ("getpeername failed, errno %d\n", errno); 
        return; 
    }

    ptr = inet_ntop (AF_INET, &addr.sin_addr, buf, INET_ADDRSTRLEN); 
    printf ("remote addr %d: %s\n", len, ptr ? ptr : "unknown"); 

}

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        printf ("Usage: dgramc port\n"); 
        return -1; 
    }

    unsigned short port = atoi (argv[1]); 
    int fd = -1; 

    do
    {
        fd = socket (AF_INET, SOCK_DGRAM, 0); 
        if (fd == -1) {
            printf ("socket call failed, errno %d\n", errno); 
            break; 
        }

        int ret = 0; 
        struct sockaddr_in addr = { 0 }; 
        addr.sin_addr.s_addr = INADDR_ANY; 
        addr.sin_port = htons (port); 

        int n = 0; 
        char dbuf[128] = { 0 }; 
        char buf[INET_ADDRSTRLEN] = { 0 }; 
        char const *ptr = NULL; 
        socklen_t len = 0; 
        while (1)
        {
            sprintf (dbuf, "this is %d", n++); 
            ret = sendto (fd, dbuf, strlen (dbuf), 0, (struct sockaddr *)&addr, sizeof (addr)); 
            if (ret == -1) { 
                printf ("sendto call failed, errno %d\n", errno); 
                break; 
            }

            printf ("send %d to %d\n", ret, port); 
            if (ret == 0) {
                printf ("no more data, quit..\n"); 
                break; 
            }

            dump_addr (fd); 
            sleep (10); 
        }
    } while (0); 

    if (fd != -1)
        close (fd); 

    return 0; 
}
