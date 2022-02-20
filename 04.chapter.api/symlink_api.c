#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 3)
        err_quit ("Usage: symlink_api actual_path symbol_path\n", 1); 

    if (symlink (argv[1], argv[2]) < 0) 
        err_sys ("%s -> %s: symlink error", argv[2], argv[1]); 

    printf ("link %s to %s\n", argv[2], argv[1]); 
    return 0; 
}


