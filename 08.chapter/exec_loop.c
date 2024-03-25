#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    char const* exename=0; 
    if (argc > 1)
        exename = argv[1]; 
    else 
        exename = "date"; 

    sleep (1); 
    printf ("%d, start exec: %s\n", getpid(), exename); 
    if (argc > 1)
        execvp (exename, &argv[1]); 
    else 
        execlp (exename, exename, (char *)0); 

    printf ("should not reach here, exec failed?\n"); 
    abort(); 
}
