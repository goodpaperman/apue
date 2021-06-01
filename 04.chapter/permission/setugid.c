#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
    int timeout = 10; 
    if (argc > 1)
        timeout = atoi(argv[1]); 

    sleep (timeout); 
    printf ("%d exit\n", getpid()); 
    return 0; 
}
