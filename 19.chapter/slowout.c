#include <stdio.h>
#include <unistd.h>

int main (void)
{
    int i = 0; 
    while (i++ < 10)
    {
        printf ("turn %d\n", i); 
        sleep (1); 
        printf ("type any char to continue\n"); 
        getchar (); 
    }
    return 0; 
}
