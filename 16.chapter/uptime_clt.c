#include "../apue.h" 
#include <netdb.h> 
#include <errno.h> 
//#include <syslog.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 

#define MAXSLEEP 128
#define MAXADDRLEN 256
#define BUFLEN 128

#define USE_UDP

#ifdef USE_UDP
#define TIMEOUT 20

void sigalrm (int signo)
{
}

#else 

int connect_retry (int sockfd, const struct sockaddr *addr, socklen_t alen)
{
    int nsec; 
    for (nsec = 1; nsec <= MAXSLEEP; nsec <<= 1) { 
        if (connect (sockfd, addr, alen) == 0) { 
            printf ("connect ok\n"); 
            return 0; 
        }

        printf ("connect failed, retry...\n"); 
        if (nsec <= MAXSLEEP/2)
            sleep (nsec); 
    }

    return -1; 
}

#endif 

void print_uptime (int sockfd)
{
    int n; 
    char buf[BUFLEN]; 
#ifdef USE_UDP
    struct sockaddr_in addr = { 0 }; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    addr.sin_port = htons (4201); 
    if (sendto (sockfd, buf, 1, 0, (struct sockaddr *)&addr, sizeof (addr)) < 0)
        err_sys ("sendto error"); 

    alarm (TIMEOUT); 
    n = recvfrom (sockfd, buf, BUFLEN, 0, NULL, NULL); 
    if (n < 0)
    {
        if (errno != EINTR)
            alarm (0); 

        err_sys ("recvfrom error"); 
    }

    alarm (0); 
    write (STDOUT_FILENO, buf, n); 
#else
    while ((n = recv (sockfd, buf, BUFLEN, 0)) > 0)
        write (STDOUT_FILENO, buf, n); 

    if (n < 0)
        err_sys ("recv error"); 
#endif
}


int main (int argc, char *argv[])
{
    int sockfd; 
#if 0
    if (argc != 2) 
        err_quit ("usage: uptime_tcp_clt hostname"); 

    int err = 0; 
    struct addrinfo *ailist, *aip; 
    struct addrinfo hint; 
    hint.ai_flags = 0; 
    hint.ai_family = 0; 
    hint.ai_socktype = SOCK_STREAM; 
    hint.ai_protocol = 0; 
    hint.ai_addrlen = 0; 
    hint.ai_canonname = NULL; 
    hint.ai_addr = NULL; 
    hint.ai_next = NULL; 
    err = getaddrinfo (argv[1], "ruptime", &hint, &ailist); 
    if (err != 0)
        err_quit("getaddrinfo error: %s", gai_strerror (err)); 

    for (aip = ailist; aip != NULL; aip = aip->ai_next) { 
        sockfd = socket (aip->ai_family, SOCK_STREAM, 0); 
        if (sockfd < 0) {
            err = errno; 
            continue; 
        }

        if (connect_retry (sockfd, aip->ai_addr, aip->ai_addrlen) < 0) {
            err = errno; 
            continue; 
        }
        else { 
            print_uptime (sockfd); 
            exit (0); 
        }
    }

    fprintf (stderr, "can't connect to %s: %s\n", argv[1], strerror (err)); 
#else
#  ifdef USE_UDP
    struct sigaction sa; 
    sa.sa_handler = sigalrm; 
    sa.sa_flags = 0; 
    sigemptyset (&sa.sa_mask); 
    if (sigaction (SIGALRM, &sa, NULL) < 0)
        err_sys ("sigaction error"); 

    sockfd = socket (AF_INET, SOCK_DGRAM, 0); 
#  else
    sockfd = socket (AF_INET, SOCK_STREAM, 0); 
#endif
    if (sockfd < 0)
        err_sys ("socket failed"); 

    print_sockopt (sockfd, "created client"); 
#  ifndef USE_UDP
    struct sockaddr_in addr = { 0 }; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    addr.sin_port = htons (4201); 
    if (connect_retry (sockfd, (struct sockaddr *) &addr, sizeof (addr)) < 0)
        err_sys ("connect failed"); 
#  endif

    print_uptime (sockfd); 
    close (sockfd); 
#endif

    exit (1); 
}
