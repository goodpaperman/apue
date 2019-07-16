#include "../apue.h" 
#include <sys/stat.h> 

//#define RW_OPEN
#define NBLK_OPEN

int main (int argc, char *argv[])
{
    char const* fifo = 0; 
    if (argc > 1)
        fifo = argv[1]; 

    if (fifo == 0)
        fifo = "fifo.tmp"; 

    if (access (fifo, W_OK) != 0) { 
        if (mkfifo (fifo, FILE_MODE) < 0)
            err_sys ("mkfifo failed"); 

        printf ("mkfifo %s", fifo); 
    }

    int flags = 0; 
#ifdef RW_OPEN
    flags = O_RDWR; 
#else
    flags = O_WRONLY; 
#endif

#ifdef NBLK_OPEN
    flags |= O_NONBLOCK; 
#endif
    int fd = open (fifo, flags); 
    if (fd < 0)
        err_sys ("open fifo for write failed"); 

    printf ("open fifo for write\n"); 
    struct stat sb; 
    if (fstat (fd, &sb) < 0)
        err_sys ("stat failed"); 

    if (!S_ISFIFO(sb.st_mode))
        err_sys ("not a fifo"); 

    printf ("is a fifo\n"); 
    int ret = 0; 
    char buf[4096] = { 0 }; 
    sprintf (buf, "this is %d", ret); 
    while ((ret = write (fd, buf, strlen(buf))) >= 0)
    {
        printf ("write %d to fifo\n", ret); 
        sprintf (buf, "this is %d", ret); 
    }

    printf ("read over\n"); 
    close (fd); 
    return 0; 
}
