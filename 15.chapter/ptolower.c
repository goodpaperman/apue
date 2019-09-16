#include "../apue.h" 
#include <sys/wait.h> 

//#define USE_REDIRECT
//#define USE_STDERR 1

int main (void)
{
    char line[MAXLINE] = { 0 }; 
#ifdef USE_REDIRECT
    FILE *fp = popen ("./tolower 2>&1", "r"); 
#else
    FILE *fp = popen ("./tolower", "r"); 
#endif

    if (fp == NULL)
        err_sys ("popen error"); 

    for (;;) { 
#ifdef USE_STDERR
        fputs ("prompt> ", stderr); 
#else
        fputs ("prompt> ", stdout); 
        fflush (stdout); 
#endif
        if (fgets (line, MAXLINE, fp) == NULL)
            break; 

#ifdef USE_STDERR
        if (fputs (line, stderr) == EOF)
#else
        if (fputs (line, stdout) == EOF)
#endif
            err_sys ("fputs error to pipe"); 
    }

    if (pclose (fp) == -1)
        err_sys ("pclose error"); 

    putchar ('\n'); 
    exit (0); 
}
