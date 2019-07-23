#include "../apue.h" 
#include <sys/stat.h> 
#include <errno.h> 

//#define MAX_LINE 10
#define MAX_LINE 3 // test write SIGPIPE
//#define RW_OPEN
//#define NBLK_OPEN

int main (int argc, char *argv[])
{
    char const* fifo = 0; 
    if (argc > 1)
        fifo = argv[1]; 

    if (fifo == 0)
        fifo = "fifo.tmp"; 

    if (access (fifo, R_OK) != 0) { 
        if (mkfifo (fifo, FILE_MODE) < 0)
            err_sys ("mkfifo failed"); 

        printf ("mkfifo %s", fifo); 
    }

    int flags = 0; 
#ifdef RW_OPEN
    flags = O_RDWR; 
#else
    flags = O_RDONLY; 
#endif

#ifdef NBLK_OPEN
    flags |= O_NONBLOCK; 
#endif
    int fd = open (fifo, flags); 
    if (fd < 0)
        err_sys ("open fifo for read failed"); 

    printf ("open fifo for read\n"); 
    struct stat sb; 
    if (fstat (fd, &sb) < 0)
        err_sys ("stat failed"); 

    if (!S_ISFIFO(sb.st_mode))
        err_sys ("not a fifo"); 

    printf ("is a fifo\n"); 
    int ret = 0, n = 0; 
    char buf[4096] = { 0 }; 
    while (n++ < MAX_LINE)
    {
        ret = read (fd, buf, sizeof buf); 
        printf ("read %d from fifo\n", ret); 
        if (ret < 0)
        {
#ifdef NBLK_OPEN
            if (errno == EAGAIN) { 
                sleep (1); 
                continue; 
            }
            else 
#endif
                break; 
        }
        else if (ret == 0) {
#ifdef NBLK_OPEN
            sleep (1); 
#else
            break; 
#endif
        }
    }

    printf ("read over, ret = %d, errno %d\n", ret, errno); 
    close (fd); 
    return 0; 
}
