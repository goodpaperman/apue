#include <signal.h> 
#include <errno.h> 
#include "../apue.h" 

int g_quit = 0; 
void sighandler (int signo)
{
    printf ("%d catch %d\n", getpid(), signo); 
    if (signo == SIGINT)
        g_quit = 1; 

    // PERSIST IT !
    signal (signo, sighandler); 
}

int main (int argc, char *argv[])
{
    pid_t pid = 0; 
    signal (SIGINT, sighandler); 
    signal (SIGQUIT, sighandler); 
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

    printf ("%d quit\n", getpid ()); 
    return 0; 
}
