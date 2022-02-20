#include "../apue.h"
#include <linux/limits.h>

int main (int argc, char *argv[])
{
    if (argc < 2)
        err_quit ("Usage: readlink_api symbol_path\n", 1); 

    char buf[PATH_MAX] = { 0 }; 
    if (readlink (argv[1], buf, sizeof(buf)) < 0) 
        err_sys ("%s: readlink error", argv[1]); 

    printf ("readlink %s:  %s\n", argv[1], buf); 
    return 0; 
}


