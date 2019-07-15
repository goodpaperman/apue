#include "../apue.h" 
#include <sys/stat.h> 

int main (int argc, char *argv[])
{
    char const* fifo = 0; 
    if (argc > 1)
        fifo = argv[1]; 

    if (fifo == 0)
        fifo = "fifo.tmp"; 

    if (access (fifo, R_OK) != 0) { 
        if (mkfifo (fifo, O_RDWR) < 0)
            err_sys ("mkfifo failed"); 

        printf ("mkfifo %s", fifo); 
    }

    int fd = open (fifo, O_RDONLY); 
    if (fd < 0)
        err_sys ("open fifo for read failed"); 

    struct stat sb; 
    if (fstat (fd, &sb) < 0)
        err_sys ("stat failed"); 

    if (!S_ISFIFO(sb.st_mode))
        err_sys ("not a fifo"); 

    int ret = 0; 
    char buf[4096] = { 0 }; 
    printf ("open fifo %s for read OK\n", fifo); 
    while ((ret = read (fd, buf, sizeof buf)) >= 0)
    {
        printf ("read %d from fifo\n", ret); 
    }

    printf ("read over\n"); 
    close (fd); 
    return 0; 
}
