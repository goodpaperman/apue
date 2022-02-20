#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 5)
        err_quit ("Usage: open_api file-path flags mode sleep\n", 1); 

    int fd = open (argv[1], atoi(argv[2]), atoi(argv[3])); 
    if (fd < 0) 
        err_sys ("%s: open error", argv[1]); 

    printf ("open %s at %d\n", argv[1], fd); 

    int sec = atoi(argv[4]);
    sleep (sec); 
    printf ("prepare to exit after %d seconds...\n", sec); 
    return 0; 
}


