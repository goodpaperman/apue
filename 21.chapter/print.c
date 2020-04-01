/* 
 * Usage: print [-t] filename
 *
 */

#include "../apue.h"
#include "print.h"
#include <fcntl.h>
#include <pwd.h>

int log_to_stderr = 1; 

void submit_file (int fd, int sockfd, const char* fname, size_t nbytes, int text)
{
    int nr, nw, len; 
    struct passwd *pwd; 
    struct printreq req; 
    struct printresp res; 
    char buf[IOBUFSZ]; 

    if ((pwd = getpwuid (geteuid ())) == NULL)
        strcpy (req.usernm, "unknown"); 
    else 
        strcpy (req.usernm, pwd->pw_name); 

    req.size = htonl (nbytes); 
    if (text)
        req.flags = htonl (PR_TEXT); 
    else 
        req.flags = 0; 

    if ((len = strlen (fname)) >= JOBNAME_MAX) {
        strcpy (req.jobnm, "... "); 
        strncat (req.jobnm, &fname[len-JOBNM_MAX+5], JOBNM_MAX-5); 
    } else {
        strcpy (req.jobnm, fname); 
    }

    nw = writen (sockfd, &req, sizeof (struct printreq)); 
    if (nw != sizeof (struct printreq)) {
        if (nw < 0)
            err_sys ("can't write to print server"); 
        else 
            err_quit ("short write (%d/%d) to print server", 
                    nw, sizeof (struct printreq)); 
    }

    while ((nr = read (fd, buf, IOBUFSZ)) != 0) { 
        nw = writen (sockfd, buf, nr); 
        if (nw != nr) {
            if (nw < 0)
                err_sys ("can't write to print server"); 
            else 
                err_quit ("short write (%d/%d) to print server", 
                        nw, nr); 
        }
    }

    if ((nr = readn (sockfd, &res, sizeof (struct printresp))) != sizeof (struct printresp))
        err_sys ("can't read response from server"); 

    if (res.retcode != 0) { 
        printf ("rejected: %s\n", res.msg); 
        exit (1); 
    } else {
        printf ("job ID %ld\n", ntohl (res.jobid)); 
    }

    exit (0); 
}

int main (int argc, char *argv[])
{
    int fd, sockfd, err, text, c; 
    struct stat sbuf; 
    char *host; 
    struct addrinfo *ailist, *aip; 

    err = 0; 
    text = 0; 
    while ((c = getopt (argc, argv, "t")) != -1) {
        switch (c) { 
            case 't':
                text = 1; 
                break; 
            case '?':
                err = 1; 
                break; 
        }
    }

    if (err || (optind != argc - 1))
        err_quit ("usage: print [-t] filename"); 

    if ((fd = open (argv[optind], O_RDONLY)) < 0)
        err_sys ("print: can't open %s", argv[optind]); 

    if (fstat (fd, &sbuf) < 0)
        err_sys ("print: can't stat %s", argv[optind]); 

    if (!S_ISREG (sbuf.st_mode))
        err_quit ("print: %s must be a regular file\n", argv[optind]); 

    if ((host = get_printserver ()) == NULL)
        err_quit ("print: no print server defined"); 

    if ((err = getaddrlist (host, "print", &ailist)) != 0)
        err_quit ("print: getaddrinfo error: %s", gai_strerror (err)); 

    for (aip = ailist; aip != NULL; aip = aip->ai_next) { 
        if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) { 
            err = errno; 
        } else if (connect_retry (sockfd, aip->ai_addr, aip->ai_addrlen) < 0) {
            err = errno; 
        } else {
            submit_file (fd, sockfd, argv[optind], sbuf.st_size, text); 
            exit (0); 
        }
    }

    errno = err; 
    err_ret ("print: can't contact %s", host); 
    exit (1); 
}
