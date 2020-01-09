#include "../apue.h"
#include "spipe.h"
#include "spipe_fd.h"
#include <errno.h>

#define CL_OPEN "open"
#define CS_OPEN "/var/opend"
#define MAXARGC 50
#define WHITE " \t\n"

int debug = 0; 
int oflag = 0; 
int client_size = 0; 
char *pathname = 0; 
char errmsg[MAXLINE]; 
extern int log_to_stderr; 

typedef struct {
    int fd; 
    uid_t uid; 
} Client; 

Client *client = NULL; 

void request (char *buf, int nread, int fd)
{
    int newfd; 
    if (buf[nread-1] != 0) { 
        sprintf (errmsg, "request not null terminated: %*.*s\n", nread, nread, buf); 
        send_err (fd, -1, errmsg); 
        return; 
    }

    if (buf_args(buf, cli_args) < 0) {
        send_err (fd, -1, errmsg); 
        return; 
    }

    if ((newfd = open (pathname, oflag)) < 0) {
        sprintf (errmsg, "can't open %s: %s\n", pathname, strerror (errno)); 
        send_err (fd, -1, errmsg); 
        return; 
    }

    if (send_fd (fd, newfd) < 0)
        err_sys ("send_fd error"); 

#if 1
    // to see if handle not closed after send..
    // yes, it is. on solaris with streams you may not need, but unix domain socket need it !!
    close (newfd); 
#endif
}

void loop ()
{
    fd_set rset, allset; 
    FD_ZERO(&allset); 

    int listenfd = serv_listen (CS_OPEN); 
    if (listenfd < 0)
        log_sys ("serv_listen error"); 

    FD_SET(listenfd, &allset); 
    int maxfd = listenfd; 
    int maxi = -1; 

    for (;;) {
        rset = allset; 
        uid_t uid = 0; 
        int clifd = 0, i = 0, nread = 0; 
        int n = select (maxfd + 1, &rset, NULL, NULL, NULL); 
        if (n < 0)
            log_sys ("select error"); 

        if (FD_ISSET(listenfd, &rset)) { 
            // accept new client request
            clifd = serv_accept (listenfd, &uid); 
            if (clifd < 0)
                log_sys ("serv_accept error: %d", clifd); 

            i = client_add (clifd, uid); 
            FD_SET (clifd, &allset); 
            if (clifd > maxfd)
                maxfd = clifd; 

            if (i > maxi)
                maxi = i; 

            log_msg ("new connection: uid %d, fd %d", uid, clifd); 
            continue; 
        }

        for (i = 0; i <= maxi; i++) { 
            clifd = client[i].fd; 
            if (clifd < 0)
                continue; 

            if (FD_ISSET(clifd, &rset)) {
                nread = read (clifd, buf, MAXLINE); 
                if (nread < 0)
                    log_sys ("read error on fd %d", clifd); 
                else if (nread == 0) {
                    log_msg ("closed: uid %d, fd %d", client[i].uid, clifd); 
                    client_del (clifd); 
                    FD_CLR (clifd, &allset); 
                    close (clifd); 
                } else {
                    request (buf, nread, clifd, client[i].uid); 
                }
            }
        }
    }
}

int main (int argc, char *argv[])
{
    int c = 0; 
    log_open ("open.serv", LOG_PID, LOG_USER); 

    opterr = 0; // don't want getopt() writting to stderr !
    while ((c = getopt (argc, argv, "d")) != EOF) {
        switch (c) { 
            case 'd':
                debug = log_to_stderr = 1; 
                break; 
            case '?':
                err_quit ("unrecongnized option: -%c", optopt); 
        }
    }

    if (debug == 0)
    {
        log_to_stderr = 0; 
        daemonize ("opend"); 
    }

    loop (); 
    return 0; 
}
