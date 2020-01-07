#include "../apue.h"
#include <unistd.h>
#include <sys/socket.h>
#include "csopen.h"
#include "spipe_fd.h"

int csopen (char *name, int oflag)
{
    pid_t pid; 
    int len = 0; 
    char buf[10] = { 0 }; 
    struct iovec iov[3]; 
    static int fd[2] = { -1, -1 }; 

    if (fd[0] < 0) {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0)
            err_sys ("socketpair error"); 

        if ((pid = fork ()) < 0) {
            err_sys ("fork error"); 
        } else if (pid == 0) {
            close (fd[0]); 
            if (fd[1] != STDIN_FILENO && dup2 (fd[1], STDIN_FILENO) != STDIN_FILENO)
                err_sys ("dup2 error to stdin"); 
            if (fd[1] != STDOUT_FILENO && dup2 (fd[1], STDOUT_FILENO) != STDOUT_FILENO)
                err_sys ("dup2 error to stdout"); 
            if (execl ("./csopend", "csopend", (char *)0) < 0)
                err_sys ("execl error"); 

            return -1; 
        }

        close (fd[1]); 
    }

    sprintf (buf, " %d", oflag); 
    iov[0].iov_base = CL_OPEN; 
    iov[0].iov_len = strlen (CL_OPEN); 
    iov[1].iov_base = name; 
    iov[1].iov_len = strlen (name); 
    iov[2].iov_base = buf; 
    iov[2].iov_len = strlen(buf)+1; // make it null terminal
    len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len; 
    if (writev (fd[0], &iov[0], 3) != len)
        err_sys ("writev error"); 

    uid_t uid = 0; 
    return recv_fd(fd[0], &uid, write); 
}
