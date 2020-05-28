/* 
 * Usage: hang [jobid]
 *
 */

#include "../apue.h"
#include "print.h"
#include <fcntl.h>
#include <pwd.h>

int log_to_stderr = 1; 

void submit_req (int sockfd, long jobid)
{
    int nr, nw, len; 
    struct passwd *pwd; 
    struct hangreq req; 
    struct hangresp res; 
    char buf[IOBUFSZ]; 

    if ((pwd = getpwuid (geteuid ())) == NULL)
        strcpy (req.usernm, "unknown"); 
    else 
        strcpy (req.usernm, pwd->pw_name); 

    req.jobid = htonl(jobid); 
    printf ("prepare to submit request: %ld\n", jobid); 
    nw = writen (sockfd, &req, sizeof (struct hangreq)); 
    if (nw != sizeof (struct hangreq)) {
        if (nw < 0)
            err_sys ("can't write to print server"); 
        else 
            err_quit ("short write (%d/%d) to print server", 
                    nw, sizeof (struct hangreq)); 
    }

    printf ("writen %d\n", nw); 
    printf ("prepare to receive response\n"); 
    if ((nr = readn (sockfd, &res, sizeof (struct hangresp))) != sizeof (struct hangresp))
        err_sys ("can't read response from server"); 

    if (res.retcode != 0) { 
        printf ("hang failed: %s\n", res.msg); 
        exit (1); 
    } else {
        printf ("msg: %s\n", res.msg); 
    }

    exit (0); 
}

int main (int argc, char *argv[])
{
    char *host; 
    if (argc > 2)
        err_quit ("usage: hang [jobid]"); 

    if ((host = get_printserver ()) == NULL)
        err_quit ("hang: no print server defined"); 

    log_msg ("print host: %s", host); 

    struct sockaddr_in addr = { 0 }; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = inet_addr (host); 
    addr.sin_port = htons(HANGSVC_PORT); 

    int sockfd = -1; 
    size_t addrlen = sizeof (addr); 
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) { 
        return errno; 
    } else if (connect_retry (sockfd, (struct sockaddr *) &addr, addrlen) < 0) {
        return errno; 
    } else {
        submit_req (sockfd, argc > 1 ? atol(argv[1]) : 0); 
        exit (0); 
    }

    return 0; 
}
