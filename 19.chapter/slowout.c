#include <stdio.h>
#include <unistd.h>

int main (void)
{
    int i = 0; 
    int tty = isatty (STDIN_FILENO); 
    printf ("stdin isatty ? %s\n", tty ? "true" : "false"); 
    tty = isatty (STDOUT_FILENO); 
    printf ("stdout isatty ? %s\n", tty ? "true" : "false"); 
    while (i++ < 10)
    {
        printf ("turn %d\n", i); 
        sleep (1); 
        printf ("type any char to continue\n"); 
#ifdef HAS_READ
        getchar (); 
#endif
    }
    return 0; 
}
