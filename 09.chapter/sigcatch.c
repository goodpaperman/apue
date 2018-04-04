#include <signal.h> 
#include <errno.h> 
#include "../apue.h" 

int g_quit = 0; 
FILE* g_log = 0; 

void sighandler (int signo)
{
    g_quit ++; 
    printf ("%d catch %d, quit %d\n", getpid(), signo, g_quit); 
    fprintf (g_log, "%d catch %d, quit %d\n", getpid(), signo, g_quit); 
    fflush (g_log); 

    //printf ("quit %d\n", g_quit); 
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
    signal (SIGTERM, sighandler); 
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

    while (g_quit < 3)
    {
        sleep (1); 
        //printf ("quit %d\n", g_quit); 
    }

    fclose (g_log); 
    printf ("%d quit %d\n", getpid (), g_quit); 
    return 0; 
}
