#include <signal.h> 
#include <errno.h> 
#include "../apue.h" 

int g_quit = 0; 
FILE* g_log = 0; 

void sighandler (int signo)
{
    g_quit ++; 
    // to test  SIGTTOU
    printf ("%d catch %d, quit %d\n", getpid(), signo, g_quit); 
    //fprintf (g_log, "%d catch %d, quit %d\n", getpid(), signo, g_quit); 
    //fflush (g_log); 

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
    //signal (SIGTTIN, SIG_IGN); 
    //signal (SIGTTOU, SIG_IGN); 
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

    int in = 0; 
    while (g_quit < 3)
    {
        sleep (10); 
        // to test  SIGTTIN
        //printf ("quit %d\n", g_quit); 
        printf ("input number: \n"); 
        scanf ("%d", &in); 
        printf ("get %d\n", in); 
    }

    fclose (g_log); 
    printf ("%d quit %d\n", getpid (), g_quit); 
    return 0; 
}
