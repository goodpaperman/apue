#include "../apue.h" 
#include <errno.h> 

int main (int argc, char *argv[])
{
    int i, fd; 
    for (i=1; i<argc; i++) { 
        fd = open (argv[i], O_RDONLY); 
        if (fd < 0) { 
            err_ret ("%s: can't open", argv[i]); 
            continue; 
        }

        if (isastream (fd) == 0)
            err_ret ("%s: not a stream, errno %d", argv[i], errno); 
        else 
            err_msg ("%s: stream device", argv[i]); 
    }

    return 0; 
}
