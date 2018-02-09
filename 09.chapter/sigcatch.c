#include <signal.h> 
#include <errno.h> 
#include "../apue.h" 

int g_quit = 0; 
FILE* g_log = 0; 
void sighandler (int signo)
{
    fprintf (g_log, "%d catch %d\n", getpid(), signo); 
    fflush (g_log); 
    if (signo == SIGHUP)
        g_quit = 1; 

    // PERSIST IT !
    signal (signo, sighandler); 
}

int main (int argc, char *argv[])
{
    pid_t pid = 0; 
    g_log = fopen ("catch.log", "a+"); 
    signal (SIGINT, sighandler); 
    signal (SIGQUIT, sighandler); 
    signal (SIGHUP, sighandler); 
    for (int i=1; i<argc; ++ i)
    {
        pid = fork (); 
        if (pid == 0)
        {
            // children
            execvp ("./sigcatch", argv+1); 
            exit (-1); 
        }
        else 
            printf ("create children %d\n", pid); 
    }

    while (!g_quit)
    {
        sleep (1); 
    }

    fclose (g_log); 
    printf ("%d quit\n", getpid ()); 
    return 0; 
}
