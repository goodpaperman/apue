#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 2)
        err_quit ("Usage: unlink_api symbol_path\n", 1); 

    if (unlink (argv[1]) < 0) 
        err_sys ("%s: unlink error", argv[1]); 

    printf ("unlink %s\n", argv[1]); 
    return 0; 
}


