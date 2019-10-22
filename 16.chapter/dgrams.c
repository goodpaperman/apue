#include "../apue.h" 
#include <sys/socket.h> 
#include <netinet/in.h> // for INADDR_ANY
#include <arpa/inet.h>  // for inet_ntop
#include <errno.h> 

//#define USE_CONN
#define USE_DISCONN
#define USE_SENDMSG

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
        printf ("Usage: connect port\n"); 
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

        struct sockaddr_in addr = { 0 }; 
        addr.sin_addr.s_addr = INADDR_ANY; 
        addr.sin_port = htons (port); 
        int ret = bind (fd, (struct sockaddr *)&addr, sizeof (addr)); 
        if (ret == -1) {
            printf ("bind call failed, errno %d\n", errno); 
            break; 
        }

        printf ("bind to %d ok\n", port); 
        char dbuf[128] = { 0 }; 
        char buf[INET_ADDRSTRLEN] = { 0 }; 
        char const* ptr = NULL; 
        socklen_t len = 0; 
        int n = 0; 
        while (1)
        {
            len = sizeof (addr); 
            printf ("ready to recv on %d\n", port); 
            ret = recvfrom (fd, dbuf, sizeof (dbuf), 0, (struct sockaddr *)&addr, &len); 
            if (ret == -1) { 
                printf ("recvfrom call failed, errno %d\n", errno); 
                break; 
            }

            ptr = inet_ntop (AF_INET, &addr.sin_addr, buf, INET_ADDRSTRLEN); 
            printf ("server recvfrom %s:%d (%d)\n", ptr, ntohs(addr.sin_port), ret); 

            dbuf[ret] = 0; 
            printf ("\t%s\n", dbuf); 
            if (ret == 0) {
                printf ("no more data, quit..\n"); 
                break; 
            }

            sprintf (dbuf, "server reply %d", n++); 

#ifdef USE_CONN
            ret = connect (fd, (struct sockaddr *) &addr, sizeof (addr)); 
            if (ret == -1) {
                printf ("connect call failed, errno %d\n", errno); 
                break; 
            }

            printf ("connect to that socket ok\n"); 
            dump_addr (fd); 

#  ifdef USE_SENDMSG
            int i = 0; 
            ptr = dbuf; 
            const int IOVSIZE = 3; 
            struct iovec iv[IOVSIZE]; 
            for (i=0; i<IOVSIZE; ++ i)
            {
                iv[i].iov_base = ptr; 
                sprintf (ptr, "this is part %d", i); 
                iv[i].iov_len = strlen (ptr) + 1; 
                ptr = ptr + strlen (ptr) + 1; 
            }

            struct msghdr mh = { 0 }; 
            // should be sockaddr_in*, otherwise got EINVAL (22)
            mh.msg_name = 0; //"hello"; 
            mh.msg_namelen = 0; // strlen (mh.msg_name);
            mh.msg_iov = iv; 
            mh.msg_iovlen = IOVSIZE;
            mh.msg_control = 0; 
            mh.msg_controllen = 0; 
            mh.msg_flags = 0; 

            ret = sendmsg (fd, &mh, 0); 
            if (ret <= 0) { 
                printf ("sendmsg call failed, errno %d\n", errno); 
                break;
            }
#  else
            // can use send now.
            ret = send (fd, dbuf, strlen (dbuf), 0); 
            if (ret <= 0) { 
                printf ("send call failed, errno %d\n", errno); 
                break;
            }
#  endif

            printf ("server send %d\n", ret); 

#  ifdef USE_DISCONN
            addr.sin_family = AF_UNSPEC; 
            addr.sin_addr.s_addr = 0; 
            addr.sin_port = 0; 
            ret = connect (fd, (struct sockaddr *)&addr, sizeof (addr)); 
            if (ret == -1) {
                printf ("disconnect failed, errno %d\n", errno); 
                break; 
            }

            printf ("disconnect that socket ok\n"); 
            dump_addr (fd); 
#  endif 
#else // USE_CONN
#  if 1
            // can use only sendto
#    ifdef USE_SENDMSG
            int i = 0; 
            ptr = dbuf; 
            const int IOVSIZE = 3; 
            struct iovec iv[IOVSIZE]; 
            for (i=0; i<IOVSIZE; ++ i)
            {
                iv[i].iov_base = ptr; 
                sprintf (ptr, "this is part %d", i); 
                iv[i].iov_len = strlen (ptr) + 1;  // contain tailing 0
                ptr = ptr + strlen (ptr) + 1; 
            }

            struct msghdr mh = { 0 }; 
            mh.msg_name = &addr; 
            mh.msg_namelen = len; 
            mh.msg_iov = iv; 
            mh.msg_iovlen = IOVSIZE;
            mh.msg_control = 0; 
            mh.msg_controllen = 0; 
            mh.msg_flags = 0; 

            ret = sendmsg (fd, &mh, 0); 
            if (ret <= 0) { 
                printf ("sendmsg call failed, errno %d\n", errno); 
                break;
            }
#    else
            // can use send now.
            ret = sendto (fd, dbuf, strlen (dbuf), 0, (struct sockaddr *)&addr, len); 
#    endif
#  else
            // will got EDESTADDRREG (89)
            ret = send (fd, dbuf, strlen (dbuf), 0); 
#  endif
            if (ret <= 0) { 
                printf ("send[to] call failed, errno %d\n", errno); 
                break;
            }

            printf ("server sendto %d\n", ret); 
#endif

            //sleep (10); 
        }
    } while (0); 

    if (fd != -1)
        close (fd); 

    return 0; 
}
