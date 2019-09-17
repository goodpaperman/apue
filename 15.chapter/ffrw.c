#include "../apue.h" 
#include <sys/stat.h> 
#include <errno.h> 
#include <pthread.h> 

#define MAX_LINE 10
//#define NBLK_OPEN
//#define OPEN_TWICE

void* thr_func (void *arg)
{
    int ret = 0, n = 0; 
    int fd = (int) arg; 
    char ibuf[4096] = { 0 }; 
    while (n++ < MAX_LINE)
    {
        ret = read (fd, ibuf, sizeof ibuf); 
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
}


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
#ifdef OPEN_TWICE
    flags = O_RDONLY | O_NONBLOCK; 
    int fd2 = open (fifo, flags); 
    if (fd2 < 0)
        err_sys ("open fifo for read async failed"); 

    printf ("open fifo for nonblock read ok\n"); 
    flags = O_WRONLY; 
    int fd = open (fifo, flags); 
    if (fd < 0)
        err_sys ("open fifo for write sync failed"); 

    printf ("open fifo for block write ok\n"); 
    clr_fl (fd2, O_NONBLOCK); 
    printf ("clear nonblock for read ok\n"); 
#else
    flags = O_RDWR; 

#  ifdef NBLK_OPEN
    flags |= O_NONBLOCK; 
#  endif
    int fd = open (fifo, flags); 
    if (fd < 0)
        err_sys ("open fifo for read/write failed"); 

    printf ("open fifo for read/write\n"); 
#endif

    struct stat sb; 
    if (fstat (fd, &sb) < 0)
        err_sys ("stat failed"); 

    if (!S_ISFIFO(sb.st_mode))
        err_sys ("not a fifo"); 

    printf ("is a fifo\n"); 
    pthread_t tid = 0; 
#ifdef OPEN_TWICE
    int ret = pthread_create (&tid, NULL, thr_func, (void *)fd2 /* for write */); 
#else
    int ret = pthread_create (&tid, NULL, thr_func, (void *)fd); 
#endif
    if (ret != 0)
    {
        printf ("pthread create failed, errno %d\n", errno); 
        close (fd); 
        return -1; 
    }

    printf ("create thread %u\n", tid); 
    int n = 0; 
    char obuf[4096] = { 0 }; 
    sprintf (obuf, "this is %d", ret); 
    while (n++ < MAX_LINE)
    {
        ret = write (fd, obuf, strlen(obuf)); 
        printf ("write %d to fifo\n", ret); 
        sleep (1); 
        sprintf (obuf, "this is %d", ret); 
    }

    printf ("write over, ret = %d, errno %d\n", ret, errno); 

    void *status = 0; 
    pthread_join (tid, &status); 
    printf ("wait write thread, return %d\n", status); 
    close (fd); 
#ifdef OPEN_TWICE
    close (fd2); 
#endif
    return 0; 
}
