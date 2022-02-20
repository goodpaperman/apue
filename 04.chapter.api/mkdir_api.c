#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 2)
        err_quit ("Usage: mkdir_api dir-name\n", 1); 

    if (mkdir (argv[1], /* 0666 */ 0755) < 0) 
        err_sys ("%s: mkdir error", argv[1]); 

    printf ("mkdir %s\n", argv[1]); 
    return 0; 
}
