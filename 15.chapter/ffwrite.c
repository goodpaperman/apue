#include "../apue.h" 
#include <sys/stat.h> 

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

    int fd = open (fifo, O_WRONLY); 
    if (fd < 0)
        err_sys ("open fifo for write failed"); 

    struct stat sb; 
    if (fstat (fd, &sb) < 0)
        err_sys ("stat failed"); 

    if (!S_ISFIFO(sb.st_mode))
        err_sys ("not a fifo"); 

    int ret = 0; 
    char buf[4096] = { 0 }; 
    printf ("open fifo %s for write  OK\n", fifo); 
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
