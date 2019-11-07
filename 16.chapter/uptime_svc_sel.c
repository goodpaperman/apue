#include "../apue.h" 
#include <netdb.h> 
#include <errno.h> 
#include <signal.h>
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <sys/select.h>

#define MAXADDRLEN 256
#define BUFLEN 128
// to test many client connect and only one can be served.
//#define QLEN 0
// seems to be no effect
#define QLEN 10

//#define USE_POPEN
#define OOB_INLINE
#define FD_SIZE 1024

int initserver (int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd; 
    int err = 0; 
    fd = socket (addr->sa_family, type, 0); 
    if (fd < 0) { 
        printf ("socket failed %d\n", errno); 
        return -1; 
    }

    int reuse = 1; 
    if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)) < 0) { 
        err = errno; 
        printf ("setsockopt error %d\n", err); 
        goto errout; 
    }

#ifdef OOB_INLINE
    int oil = 1; 
    if (setsockopt (fd, SOL_SOCKET, SO_OOBINLINE, &oil, sizeof (oil)) < 0) { 
        err = errno; 
        printf ("setsockopt error %d\n", err); 
        goto errout; 
    }
#endif

    if (bind (fd, addr, alen) < 0) { 
        err = errno; 
        printf ("bind error %d\n", err); 
        goto errout; 
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) { 
        if (listen (fd, qlen) < 0) { 
            err = errno; 
            printf ("listen error %d\n", err); 
            goto errout; 
        }
    }

    return fd; 

errout:
    close (fd); 
    errno = err; 
    return -1; 
}

void do_uptime (int clfd)
{
    int ret = 0; 
#ifdef USE_POPEN
    char buf[BUFLEN]; 
    FILE *fp = popen ("/usr/bin/uptime", "r"); 
    if (fp == NULL) { 
        sprintf (buf, "error: %s\n", strerror (errno)); 
        ret = send (clfd, buf, strlen (buf), 0); 

        // see comments below
        printf ("write back %d for error\n", ret); 
    } else { 
        // it seems no effect, as listen will automatically
        // accept new connection and client will send ok...
#  if 0
        // to test handle multiple connection once..
        sleep (10); 
#  endif
        while (fgets (buf, BUFLEN, fp) != NULL) 
        {
            ret = send (clfd, buf, strlen (buf), 0); 
            printf ("write back %d\n", ret); 
        }

        pclose (fp); 
    }

    close (clfd); 

#else  // USE_POPEN

    pid_t pid = fork (); 
    if (pid < 0) { 
        printf ("fork error: %s\n", strerror (errno)); 
        exit (1); 
    }
    else if (pid == 0) { 
        // child
        if ((dup2 (clfd, STDOUT_FILENO) != STDOUT_FILENO) ||
            (dup2 (clfd, STDERR_FILENO) != STDERR_FILENO)) 
        { 
            printf ("redirect stdout/err to socket failed\n"); 
            exit (1); 
        }
        
        // it seems no effect, as listen will automatically
        // accept new connection and client will send ok...
#  if 0
        // to test handle multiple connection once..
        sleep (10); 
#  endif

        close (clfd); 
#  if 0
        // to test handle multiple connection once..
        system ("sleep 10 & uptime"); 
#  else
        execl ("/usr/bin/uptime", "uptime", (char *)0); 
#  endif
        printf ("unexpected return from exec: %s\n", strerror (errno)); 
    }
    else 
    {
        close (clfd); 
        int status = 0; 
        ret = waitpid (pid, &status, 0); 
        printf ("wait child %d return %d: %d\n", pid, ret, status); 
    }

#endif 
}

void serve (int sockfd)
{
    int ret; 
    int clfd; 

    char buf[BUFLEN]; 
    fd_set cltds, rdds, exds; 
    FD_ZERO(&cltds); 
    FD_ZERO(&rdds); 
    FD_ZERO(&exds); 

    for (;;) { 
        // must set it in every loop.
        memcpy (&rdds, &cltds, sizeof (cltds)); 
        memcpy (&exds, &cltds, sizeof (cltds)); 
        //for (clfd = 0; clfd<FD_SIZE; ++ clfd)
        //{
        //    if (FD_ISSET(clfd, &cltds))
        //    {
        //        FD_SET(clfd, &rdds); 
        //        FD_SET(clfd, &exds); 
        //    }
        //}

        FD_SET(sockfd, &rdds); 
        ret = select (FD_SIZE+1, &rdds, NULL, &exds, NULL); 
        // will clear allds & tv after this call.
		if (ret == -1) { 
            err_sys ("select error"); 
        } else if (ret == 0){ 
		    printf ("select over but no event\n"); 
            continue; 
        }  

        printf ("got event %d\n", ret); 
        for (clfd=0; clfd<FD_SIZE; ++ clfd)
        {
            if (FD_ISSET(clfd, &rdds))
            {
               if (clfd == sockfd)
               {
                   // the acceptor
                    printf ("poll accept in\n"); 
                    clfd = accept (sockfd, NULL, NULL); 
                    if (clfd < 0) { 
                        printf ("accept error: %d, %s\n", errno, strerror (errno)); 
                        exit (1); 
                    }

                    print_sockopt (clfd, "new accepted client"); 
                    // remember it
                    FD_SET(clfd, &cltds); 
                    printf ("add %d to client set\n", clfd); 
               } 
               else 
               {
                   // the normal client
                   printf ("poll read in\n"); 
                   ret = recv (clfd, buf, sizeof(buf), 0); 
                   if (ret > 0)
                       buf[ret] = 0; 
                   else 
                       sprintf (buf, "errno %d", errno); 

                   printf ("recv %d from %d: %s\n", ret, clfd, buf); 
                   if (ret <= 0) {
                       FD_CLR(clfd, &cltds); 
                       printf ("remove %d from client set\n", clfd); 
                   }
               }
            }

            if (FD_ISSET(clfd, &exds))
            {
                // the oob from normal client
                printf ("poll exception in\n"); 
                if (sockatmark (clfd))
                {
                    printf ("has oob!\n"); 
#  ifdef OOB_INLINE
                    // no MSG_OOB, as OOB are treated as common data
                    ret = recv (clfd, buf, 1, 0); 
#  else
                    ret = recv (clfd, buf, 1, MSG_OOB); 
#  endif        
                    if (ret > 0)
                        buf[ret] = 0; 
                    else 
                       sprintf (buf, "errno %d", errno); 

                    printf ("recv %d from %d on urgent: %s\n", ret, clfd, buf); 
                    if (ret > 0) 
                       do_uptime (clfd); 

                    // after do_uptime, clfd is closed
                    FD_CLR(clfd, &cltds); 
                    printf ("remove %d from client set\n", clfd); 
                }
                else 
                    printf ("no oob!\n"); 
            }
        }
    }
}

int main (int argc, char *argv[])
{
    int sockfd, n; 
    char *host; 

    if (argc != 1) 
        err_quit ("usage: ruptimed"); 

    n = sysconf (_SC_HOST_NAME_MAX); 
    if (n < 0)
        err_quit ("no _SC_HOST_NAME_MAX defined"); 

    printf ("hostname length: %d\n", n); 
    host = malloc (n); 
    if (host == NULL)
        err_sys ("malloc error"); 

    if (gethostname (host, n) < 0)
        err_sys ("gethostname error"); 

    printf ("get hostname: %s\n", host); 

    struct sockaddr_in addr = { 0 }; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = INADDR_ANY; 
    addr.sin_port = htons (4201); 
    sockfd = initserver (SOCK_STREAM, (const struct sockaddr *)&addr, sizeof (addr), QLEN); 
    print_sockopt (sockfd, "server socket"); 
    if (sockfd >= 0)
        serve (sockfd); 
    else
        printf ("init server socket failed\n"); 

    free (host); 
    exit (1); 
}
