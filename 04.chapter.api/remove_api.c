#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 2)
        err_quit ("Usage: remove_api symbol_path\n", 1); 

    if (remove (argv[1]) < 0) 
        err_sys ("%s: remove error", argv[1]); 

    printf ("remove %s\n", argv[1]); 
    return 0; 
}


