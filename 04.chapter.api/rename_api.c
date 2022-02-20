#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 3)
        err_quit ("Usage: rename_api old_path new_path\n", 1); 

    if (rename (argv[1], argv[2]) < 0) 
        err_sys ("%s -> %s: rename error", argv[1], argv[2]); 

    printf ("rename %s to %s\n", argv[1], argv[2]); 
    return 0; 
}


