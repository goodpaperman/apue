#include "../apue.h" 
#include <netdb.h> 
#include <errno.h> 
#include <signal.h>
#include <syslog.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 

#define MAXADDRLEN 256
#define BUFLEN 128
#define QLEN 10

//#define USE_DAEMON
#define USE_POPEN

// can only define USE_UDP with USE_POPEN
#ifdef USE_POPEN
//#  define USE_UDP
#endif 

#ifndef USE_UDP
#  define OOB_RCV
#  define OOB_INLINE
#endif

#ifndef OOB_INLINE
int g_fd = 0; 
void on_urg (int signo)
{
    int ret = 0; 
    char buf[BUFLEN] = { 0 }; 
    ret = recv (g_fd, buf, sizeof (buf), MSG_OOB); 
    if (ret > 0)
        buf[ret] = 0; 
    else 
        strcpy (buf, "n/a"); 

    //syslog (LOG_INFO, "got urgent data on signal %d, len %d, %s\n", signo, ret, buf); 
    printf ("got urgent data on signal %d, len %d, %s\n", signo, ret, buf); 

}
#endif

int initserver (int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd; 
    int err = 0; 
    fd = socket (addr->sa_family, type, 0); 
    if (fd < 0) { 
        //syslog (LOG_ERR, "socket failed %d", errno); 
        printf ("socket failed %d\n", errno); 
        return -1; 
    }

    int reuse = 1; 
    if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)) < 0) { 
        err = errno; 
        //syslog (LOG_ERR, "setsockopt error %d", err); 
        printf ("setsockopt error %d\n", err); 
        goto errout; 
    }

#ifdef OOB_INLINE
    int oil = 1; 
    if (setsockopt (fd, SOL_SOCKET, SO_OOBINLINE, &oil, sizeof (oil)) < 0) { 
        err = errno; 
        //syslog (LOG_ERR, "setsockopt error %d", err); 
        printf ("setsockopt error %d\n", err); 
        goto errout; 
    }
#else

#  if 0
    // use signal
    signal (SIGURG, on_urg);
#  else 
    struct sigaction sa; 
    sa.sa_handler = on_urg; 
    sa.sa_flags |= SA_RESTART; 
    sigemptyset (&sa.sa_mask); 
    sigaction (SIGURG, &sa, NULL); 
#  endif 

    //syslog (LOG_INFO, "setup SIGURG for oob data\n"); 
    printf ("setup SIGURG for oob data\n"); 
#endif

    //syslog (LOG_INFO, "setown to %d", getpid ()); 
    printf ("setown to %d\n", getpid ()); 
    if (bind (fd, addr, alen) < 0) { 
        err = errno; 
        //syslog (LOG_ERR, "bind error %d", err); 
        printf ("bind error %d\n", err); 
        goto errout; 
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) { 
        if (listen (fd, qlen) < 0) { 
            err = errno; 
            //syslog (LOG_ERR, "listen error %d", err); 
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
#ifdef USE_UDP
        alen = sizeof (addr); 
        ret = recvfrom (sockfd, buf, BUFLEN, 0, (struct sockaddr *)&addr, &alen); 
        if (ret < 0) { 
            //syslog (LOG_ERR, "recvfrom error: %s", strerror (errno)); 
            printf ("recvfrom error: %s\n", strerror (errno)); 
            exit (1); 
        }
#else
        clfd = accept (sockfd, NULL, NULL); 
        if (clfd < 0) { 
            //syslog (LOG_ERR, "accept error: %d, %s", errno, strerror (errno)); 
            printf ("accept error: %d, %s\n", errno, strerror (errno)); 
            //if (errno == EOPNOTSUPP)
            //{
            //    syslog (LOG_INFO, "retry.."); 
            //    continue; 
            //}

            exit (1); 
        }

#ifndef OOB_INLINE
        g_fd = clfd; 
        if (fcntl (clfd, F_SETOWN, getpid ()) < 0) { 
            ////syslog (LOG_ERR, "fcntl SETOWN error %d", errno); 
            printf ("fcntl SETOWN error %d\n", errno); 
            exit (1); 
        }
#endif

        print_sockopt (clfd, "new accepted client"); 
#  ifdef OOB_RCV
        ret = recv (clfd, buf, sizeof(buf), 0); 
        if (ret > 0)
            buf[ret] = 0; 
        else 
            strcpy (buf, "n/a"); 

        printf ("recv %d: %s\n", ret, buf); 
        if (sockatmark (clfd))
        {
            printf ("has oob!\n"); 
#  ifdef OOB_INLINE
            // no MSG_OOB, as OOB are treated as common data
            ret = recv (clfd, buf, sizeof(buf), 0); 
#  else
            ret = recv (clfd, buf, sizeof(buf), MSG_OOB); 
#  endif
            if (ret > 0)
                buf[ret] = 0; 
            else 
                strcpy (buf, "n/a"); 

            printf ("recv %d: %s\n", ret, buf); 
        }
        else 
            printf ("no oob!\n"); 

        ret = recv (clfd, buf, sizeof(buf), 0); 
        if (ret > 0)
            buf[ret] = 0; 
        else 
            strcpy (buf, "n/a"); 

        printf ("recv %d: %s\n", ret, buf); 
#  endif
#endif

#ifdef USE_POPEN
        fp = popen ("/usr/bin/uptime", "r"); 
        if (fp == NULL) { 
            sprintf (buf, "error: %s\n", strerror (errno)); 
#  ifdef USE_UDP
            ret = sendto (sockfd, buf, strlen (buf), 0, (struct sockaddr *)&addr, alen); 
#  else
            ret = send (clfd, buf, strlen (buf), 0); 
#  endif 

            // see comments below
            ////syslog (LOG_ERR, "write back %d for error", ret); 
            printf ("write back %d for error\n", ret); 
        } else { 
            while (fgets (buf, BUFLEN, fp) != NULL) 
            {
#  ifdef USE_UDP
                ret = sendto (sockfd, buf, strlen (buf), 0, (struct sockaddr *)&addr, alen); 
#  else
                ret = send (clfd, buf, strlen (buf), 0); 
#  endif

                // very amazing, add these log will lead to accept failed with EOPNOTSUPP (95)
                // maybe syslog used dgram socket confuse us..
                ////syslog (LOG_ERR, "write back %d", ret); 
                printf ("write back %d\n", ret); 
            }

            pclose (fp); 
        }

        close (clfd); 

#else  // USE_POPEN

        pid = fork (); 
        if (pid < 0) { 
            //syslog (LOG_ERR, "fork error: %s", strerror (errno)); 
            printf ("fork error: %s\n", strerror (errno)); 
            exit (1); 
        }
        else if (pid == 0) { 
            // child
            if ((dup2 (clfd, STDOUT_FILENO) != STDOUT_FILENO) ||
                (dup2 (clfd, STDERR_FILENO) != STDERR_FILENO)) 
            { 
                //syslog (LOG_ERR, "redirect stdout/err to socket failed"); 
                printf ("redirect stdout/err to socket failed\n"); 
                exit (1); 
            }
            
            close (clfd); 
            execl ("/usr/bin/uptime", "uptime", (char *)0); 
            //syslog (LOG_ERR, "unexpected return from exec: %s", strerror (errno)); 
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

    //syslog (LOG_INFO, "hostname length: %d", n); 
    printf ("hostname length: %d\n", n); 
    host = malloc (n); 
    if (host == NULL)
        err_sys ("malloc error"); 

    if (gethostname (host, n) < 0)
        err_sys ("gethostname error"); 

#ifdef USE_DAEMON
    daemonize ("ruptimed"); 
#endif
    //syslog (LOG_INFO, "get hostname: %s\n", host); 
    printf ("get hostname: %s\n", host); 

#if 0
    int err = 0; 
    struct addrinfo *ailist, *aip; 
    struct addrinfo hint; 
    hint.ai_flags = AI_CANONNAME; 
    hint.ai_family = 0; 
    hint.ai_socktype = SOCK_STREAM; 
    hint.ai_protocol = 0; 
    hint.ai_addrlen = 0; 
    hint.ai_canonname = NULL; 
    hint.ai_addr = NULL; 
    hint.ai_next = NULL; 
    err = getaddrinfo (host, "ruptime", &hint, &ailist); 
    if (err != 0) { 
        syslog (LOG_ERR, "getaddrinfo error: %s", gai_strerror (err)); 
        exit (1); 
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) { 
        sockfd = initserver (SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN); 
        if (sockfd >= 0) { 
            serve (sockfd); 
            exit (0); 
        }
    }
#else
    struct sockaddr_in addr = { 0 }; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = INADDR_ANY; 
    addr.sin_port = htons (4201); 
#  ifdef USE_UDP
    sockfd = initserver (SOCK_DGRAM, (const struct sockaddr *)&addr, sizeof (addr), 0); 
#  else
    sockfd = initserver (SOCK_STREAM, (const struct sockaddr *)&addr, sizeof (addr), QLEN); 
#  endif
    print_sockopt (sockfd, "server socket"); 
    if (sockfd >= 0)
        serve (sockfd); 
    else  {
        //syslog (LOG_ERR, "init server socket failed"); 
        printf ("init server socket failed\n"); 
    }
#endif

    free (host); 
    exit (1); 
}
