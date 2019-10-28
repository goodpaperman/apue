#include "../apue.h" 
#include <netdb.h> 
#include <errno.h> 
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
#  define USE_UDP
#endif 

int initserver (int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd; 
    int err = 0; 
    fd = socket (addr->sa_family, type, 0); 
    if (fd < 0) { 
        syslog (LOG_ERR, "socket failed %d", errno); 
        return -1; 
    }

    int reuse = 1; 
    if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)) < 0) { 
        err = errno; 
        syslog (LOG_ERR, "setsockopt error %d", err); 
        goto errout; 
    }

    if (bind (fd, addr, alen) < 0) { 
        err = errno; 
        syslog (LOG_ERR, "bind error %d", err); 
        goto errout; 
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) { 
        if (listen (fd, qlen) < 0) { 
            err = errno; 
            syslog (LOG_ERR, "listen error %d", err); 
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
            syslog (LOG_ERR, "recvfrom error: %s", strerror (errno)); 
            exit (1); 
        }
#else
        clfd = accept (sockfd, NULL, NULL); 
        if (clfd < 0) { 
            syslog (LOG_ERR, "accept error: %d, %s", errno, strerror (errno)); 
            //if (errno == EOPNOTSUPP)
            //{
            //    syslog (LOG_INFO, "retry.."); 
            //    continue; 
            //}

            exit (1); 
        }

        print_sockopt (clfd, "new accepted client"); 
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
#  ifndef USE_DAEMON
            syslog (LOG_ERR, "write back %d for error", ret); 
#  endif
        } else { 
            while (fgets (buf, BUFLEN, fp) != NULL) 
            {
#  ifdef USE_UDP
                ret = sendto (sockfd, buf, strlen (buf), 0, (struct sockaddr *)&addr, alen); 
#  else
                ret = send (clfd, buf, strlen (buf), 0); 
#  endif

#  ifndef USE_DAEMON
                // very amazing, add these log will lead to accept failed with EOPNOTSUPP (95)
                // maybe syslog used dgram socket confuse us..
                syslog (LOG_ERR, "write back %d", ret); 
#  endif
            }

            pclose (fp); 
        }

        close (clfd); 

#else  // USE_POPEN

        pid = fork (); 
        if (pid < 0) { 
            syslog (LOG_ERR, "fork error: %s", strerror (errno)); 
            exit (1); 
        }
        else if (pid == 0) { 
            // child
            if ((dup2 (clfd, STDOUT_FILENO) != STDOUT_FILENO) ||
                (dup2 (clfd, STDERR_FILENO) != STDERR_FILENO)) 
            { 
                syslog (LOG_ERR, "redirect stdout/err to socket failed"); 
                exit (1); 
            }
            
            close (clfd); 
            execl ("/usr/bin/uptime", "uptime", (char *)0); 
            syslog (LOG_ERR, "unexpected return from exec: %s", strerror (errno)); 
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

    syslog (LOG_INFO, "hostname length: %d", n); 
    host = malloc (n); 
    if (host == NULL)
        err_sys ("malloc error"); 

    if (gethostname (host, n) < 0)
        err_sys ("gethostname error"); 

    syslog (LOG_INFO, "get hostname: %s\n", host); 
#ifdef USE_DAEMON
    daemonize ("ruptimed"); 
#endif

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
    else 
        syslog (LOG_ERR, "init server socket failed"); 
#endif

    free (host); 
    exit (1); 
}
