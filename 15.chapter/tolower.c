#include "../apue.h" 
#include <ctype.h> 

#define USE_STDERR 1

int main (void) 
{
    int c; 
    while ((c = getchar ()) != EOF) { 
        if (isupper (c))
            c = tolower (c); 

#ifdef USE_STDERR
        if (fputc (c, stderr) == EOF)
            err_sys ("output error"); 
#else
        if (putchar (c) == EOF)
            err_sys ("output error"); 
        if (c == '\n')
            fflush (stdout); 
#endif
    }

    return 0; 
}
