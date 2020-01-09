#include "../apue.h"
#include <unistd.h>
#include <sys/socket.h>
#include "csopen2.h"
#include "spipe.h"
#include "spipe_fd.h"

int csopen (char *name, int oflag)
{
    int len = 0; 
    char buf[10] = { 0 }; 
    struct iovec iov[3]; 
    static int csfd = -1; 

    if (csfd < 0) {
        if ((csfd = cli_conn (CS_OPEN)) < 0)
            err_sys ("cli_conn error"); 
    }

    sprintf (buf, " %d", oflag); 
    iov[0].iov_base = CL_OPEN; 
    iov[0].iov_len = strlen (CL_OPEN); 
    iov[1].iov_base = name; 
    iov[1].iov_len = strlen (name); 
    iov[2].iov_base = buf; 
    iov[2].iov_len = strlen(buf)+1; // make it null terminal
    len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len; 
    if (writev (csfd, &iov[0], 3) != len)
        err_sys ("writev error"); 

    uid_t uid = 0; 
    return recv_fd(csfd, &uid, write); 
}
