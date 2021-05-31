#include <stdio.h>
#include <stdlib.h>

int main (void)
{
    sleep (10); 
    printf ("%d exit\n", getpid()); 
    return 0; 
}
