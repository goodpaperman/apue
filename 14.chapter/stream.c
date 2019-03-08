#include <errno.h> 
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>

int main (int argc, char *argv[])
{
    int i, fd; 
    for (i=1; i<argc; i++) { 
        fd = open (argv[i], O_RDONLY); 
        if (fd < 0) { 
            printf ("%s: can't open\n", argv[i]); 
            continue; 
        }

        if (isastream (fd) == 0)
            printf ("%s: not a stream, errno %d\n", argv[i], errno); 
        else 
            printf ("%s: stream device\n", argv[i]); 
    }

    return 0; 
}
