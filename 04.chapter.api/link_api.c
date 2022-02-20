#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 3)
        err_quit ("Usage: link_api existing_path new_path\n", 1); 

    if (link (argv[1], argv[2]) < 0) 
        err_sys ("%s -> %s: link error", argv[2], argv[1]); 

    printf ("link %s to %s\n", argv[2], argv[1]); 
    return 0; 
}


