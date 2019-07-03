#include "../apue.h" 
#include <sys/wait.h> 

int main (void)
{
    char line[MAXLINE] = { 0 }; 
    FILE *fp = popen ("./tolower", "r"); 
    if (fp == NULL)
        err_sys ("popen error"); 

    for (;;) { 
        fputs ("prompt> ", stdout); 
        fflush (stdout); 
        if (fgets (line, MAXLINE, fp) == NULL)
            break; 
        if (fputs (line, stdout) == EOF)
            err_sys ("fputs error to pipe"); 
    }

    if (pclose (fp) == -1)
        err_sys ("pclose error"); 

    putchar ('\n'); 
    exit (0); 
}
