#include <errno.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>
#include <string.h>

int my_isastream (int fd)
{
    return (ioctl(fd, I_CANPUT, 0) != -1); 
}

int main (int argc, char *argv[])
{
    int i, fd; 
    for (i=1; i<argc; i++) { 
        fd = open (argv[i], O_RDONLY); 
        if (fd < 0) { 
            printf ("%s: can't open\n", argv[i]); 
            continue; 
        }

#if 0
        if (isastream (fd) == 0)
#else
        if (my_isastream (fd) == 0)
#endif
            printf ("%s: not a stream, errno %d, msg %s\n", argv[i], errno, strerror(errno)); 
        else 
            printf ("%s: stream device\n", argv[i]); 
    }

    return 0; 
}
