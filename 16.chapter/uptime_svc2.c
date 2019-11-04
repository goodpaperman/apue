#include "../apue.h" 
#include <netdb.h> 
#include <errno.h> 
#include <signal.h>
#include <sys/socket.h> 
#include <sys/wait.h> 

// the only diff from uptime_svc2 to uptime_svc is:
//   we use asynchronize socket IO mechanism instead of synchronize
//
#define MAXADDRLEN 256
#define BUFLEN 128
#define QLEN 10

#define USE_POPEN
#define USE_SIGIO
#define USE_ASYNC

// note in linux, 
// there are different value:
// O_ASYNC : 20000 - using for SIGIO notify
// O_NONBLOCK : 04000 - using for recv/send nonblocking
#if 0
#  define ASYNC_FLAG O_NONBLOCK
#else
#  define ASYNC_FLAG O_ASYNC | O_NONBLOCK
#endif

#ifdef USE_SIGIO
int g_fd = 0; 
void on_sockio (int signo)
{
    int ret = 0; 
    char buf[BUFLEN] = { 0 }; 
    ret = recv (g_fd, buf, sizeof (buf), 0); 
    if (ret > 0)
        buf[ret] = 0; 
    else {
        ret = errno; 
        strcpy (buf, "n/a"); 
    }

    printf ("recv on signal %d, len %d, %s\n", signo, ret, buf); 

}
#endif

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

#ifdef USE_SIGIO
    struct sigaction sa; 
    sa.sa_handler = on_sockio; 
    //sa.sa_flags |= SA_RESTART; 
    sa.sa_flags = 0; 
    sigemptyset (&sa.sa_mask); 
    sigaction (SIGIO, &sa, NULL); 

    printf ("setup SIGIO for asynchronize receving data\n"); 
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

void serve (int sockfd)
{
    int ret; 
    int clfd; 
    int status; 

    FILE *fp; 
    pid_t pid; 
    char buf[BUFLEN]; 
    //char abuf[MAXADDRLEN]; 
    struct sockaddr_in addr; 
    socklen_t alen; // = sizeof (addr); 

    for (;;) { 
        clfd = accept (sockfd, NULL, NULL); 
        if (clfd < 0) { 
            printf ("accept error: %d, %s\n", errno, strerror (errno)); 
            //if (errno == EOPNOTSUPP)
            //{
            //    printf ("retry.."); 
            //    continue; 
            //}

            exit (1); 
        }

#ifdef USE_SIGIO
        g_fd = clfd; 
        if (fcntl (clfd, F_SETOWN, getpid ()) < 0) { 
            printf ("fcntl SETOWN error %d\n", errno); 
            exit (1); 
        }

#  ifdef USE_ASYNC
        int flags = fcntl (clfd, F_GETFL); 
        //printf ("got fd flags: 0x%08x\n", flags); 
        if (fcntl (clfd, F_SETFL, flags | ASYNC_FLAG) < 0) { 
            printf ("fcntl with F_SETFL error %d\n", errno); 
            exit (1); 
        }
#  else
        //still need O_NONBLOCK for recv in signal and main.
        int flags = fcntl (clfd, F_GETFL); 
        if (fcntl (clfd, F_SETFL, flags | O_NONBLOCK) < 0) { 
            printf ("fcntl with F_SETFL error %d\n", errno); 
            exit (1); 
        }

        int n = 1; 
        ret = ioctl (clfd, FIOASYNC, &n); 
        if (ret < 0) {
            printf ("ioctl with FIOASYNC error %d\n", errno); 
            exit (1); 
        }
#  endif
#endif

        print_sockopt (clfd, "new accepted client"); 
#  ifdef USE_SIGIO
#    if 1
        ret = recv (clfd, buf, sizeof(buf), 0); 
        if (ret > 0)
            buf[ret] = 0; 
        else  {
            ret = errno; 
            strcpy (buf, "n/a"); 
        }

        printf ("main recv %d: %s\n", ret, buf); 
#    else
        sleep (10); 
#    endif
#endif

#ifdef USE_POPEN
        fp = popen ("/usr/bin/uptime", "r"); 
        if (fp == NULL) { 
            sprintf (buf, "error: %s\n", strerror (errno)); 
            ret = send (clfd, buf, strlen (buf), 0); 

            // see comments below
            printf ("write back %d for error\n", ret); 
        } else { 
            while (fgets (buf, BUFLEN, fp) != NULL) 
            {
                ret = send (clfd, buf, strlen (buf), 0); 
                printf ("write back %d\n", ret); 
            }

            pclose (fp); 
        }

        close (clfd); 

#else  // USE_POPEN

        pid = fork (); 
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
            
            close (clfd); 
            execl ("/usr/bin/uptime", "uptime", (char *)0); 
            printf ("unexpected return from exec: %s\n", strerror (errno)); 
        }
        else 
        {
            close (clfd); 
            waitpid (pid, &status, 0); 
        }

#endif 
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
    else  {
        printf ("init server socket failed\n"); 
    }

    free (host); 
    exit (1); 
}
