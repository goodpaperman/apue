#include "../apue.h" 
#include <errno.h> 
#include <fcntl.h> 
#include <unistd.h> 

#define USE_NONBLOCK
char buf[5]; 

int main ()
{
    int ntotal, nwrite; 
    int fd[2] = { 0 }; 
    if (pipe (fd) < 0)
    {
        fprintf (stderr, "pipe failed, errno %d\n", errno); 
        return -1; 
    }

#ifdef USE_NONBLOCK
    set_fl (fd[1], O_NONBLOCK); 
#endif
    while (1) { 
        nwrite = write (fd[1], buf, 1); 
        if (nwrite > 0) { 
            ntotal += nwrite; 
#ifndef USE_NONBLOCK
            fprintf (stderr, "write to pipe %d\n", ntotal); 
#endif
        }
        else {
            fprintf (stderr, "write to pipe failed, errno %d\n", errno); 
            break;
        }
    }

    fprintf (stderr, "total write = %d, errno = %d\n", ntotal, errno); 
#ifdef USE_NONBLOCK
    clr_fl (fd[1], O_NONBLOCK); 
#endif

    close (fd[0]); 
    close (fd[1]); 
    exit (0); 
}
