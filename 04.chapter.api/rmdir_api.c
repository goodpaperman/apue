#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 2)
        err_quit ("Usage: rmdir_api dir-name\n", 1); 

    if (rmdir (argv[1]) < 0) 
        err_sys ("%s: rmdir error", argv[1]); 

    printf ("rmdir %s\n", argv[1]); 
    return 0; 
}
