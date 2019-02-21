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
            printf ("%s: can't open", argv[i]); 
            continue; 
        }

        if (isastream (fd) == 0)
            printf ("%s: not a stream, errno %d", argv[i], errno); 
        else 
            printf ("%s: stream device", argv[i]); 
    }

    return 0; 
}
