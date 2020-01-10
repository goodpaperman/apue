#include "../apue.h"
#include "spipe.h"
#include "spipe_fd.h"
#include <errno.h>
#include <sys/select.h>
#include <syslog.h>

#define CL_OPEN "open"
#define CS_OPEN "/tmp/opend"
#define MAXARGC 50
#define WHITE " \t\n"
#define NALLOC 10

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

static void client_alloc (void)
{
    int i = 0; 
    if (client == NULL)
        client = malloc (NALLOC * sizeof (Client)); 
    else 
        client = realloc (client, (client_size + NALLOC) * sizeof(Client)); 

    if (client == NULL)
        log_sys ("can't alloc for client array"); 

    for (i = client_size; i < client_size + NALLOC; ++ i)
        client[i].fd = -1; 

    client_size += NALLOC; 
}


int client_add (int fd, uid_t uid)
{
    int i = 0; 
    if (client == NULL)
        client_alloc (); 

    while (1)
    {
        for (i = 0; i<client_size; ++ i) { 
            if (client[i].fd == -1) { 
                client[i].fd = fd; 
                client[i].uid = uid; 
                return i; 
            }
        }

        client_alloc (); 
    }

    return -1; 
}

void client_del (int fd)
{
    int i = 0; 
    for (i = 0; i<client_size; ++ i) {
        if (client[i].fd == fd) { 
            client[i].fd = -1; 
            return; 
        }
    }

    log_quit ("can't find client entry for fd %d", fd); 
}

int cli_args (int argc, char **argv)
{
    if (argc != 3 || strcmp (argv[0], CL_OPEN) != 0) {
        strcpy (errmsg, "usage: open <pathname> <oflag>\n"); 
        return -1; 
    }

    pathname = argv[1]; 
    oflag = atoi (argv[2]); 
    return 0; 
}

int buf_args (char *buf, int (*optfunc)(int, char **))
{
    char *ptr, *argv[MAXARGC]; 
    int argc = 0; 

    if (strtok (buf, WHITE) == NULL) return -1; 

    argv[argc] = buf; 
    while ((ptr = strtok (NULL, WHITE)) != NULL) {
        if (++ argc >= MAXARGC-1)
            return -1; 

        argv[argc] = ptr; 
    }

    argv[++argc] = NULL; 
    return optfunc(argc, argv); 
}

void request (char *buf, int nread, int fd, uid_t uid)
{
    int newfd; 
    if (buf[nread-1] != 0) { 
        sprintf (errmsg, "request from uid %d not null terminated: %*.*s\n", uid, nread, nread, buf); 
        send_err (fd, -1, errmsg); 
        log_msg (errmsg); 
        return; 
    }

    log_msg ("request: %s, from uid %d", buf, uid); 
    if (buf_args(buf, cli_args) < 0) {
        send_err (fd, -1, errmsg); 
        log_msg (errmsg); 
        return; 
    }

    if ((newfd = open (pathname, oflag)) < 0) {
        sprintf (errmsg, "can't open %s: %s\n", pathname, strerror (errno)); 
        send_err (fd, -1, errmsg); 
        log_msg (errmsg); 
        return; 
    }

    if (send_fd (fd, newfd) < 0)
        log_sys ("send_fd error"); 

    log_msg ("sent fd %d over fd %d for %s", newfd, fd, pathname); 
    close (newfd); 
}

void loop ()
{
    fd_set rset, allset; 
    FD_ZERO(&allset); 
    char buf[MAXLINE]; 

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
