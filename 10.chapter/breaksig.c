#include "../apue.h" 
#include <strings.h> 
#include <signal.h>

static void sigalrm (int signo)
{
    // do nothing, just break read call !
}

int main (int argc, char *argv[])
{
    int n; 
    char line [MAXLINE]; 
#if 0
    if (signal (SIGALRM, sigalrm) == SIG_ERR)
        err_sys ("signal (SIGALRM) error"); 
#else 
    struct sigaction sa; 
    sa.sa_flags = SA_RESTART; 
    sigemptyset (&sa.sa_mask); 
    sa.sa_handler = sigalrm; 
    sigaction (SIGALRM, &sa, NULL); 
#endif

    do 
    {
        alarm (10); 
        if ((n = read (STDIN_FILENO, line, MAXLINE)) < 0)
            err_sys ("read error"); 

        if (strcasecmp (line, "exit") == 0)
            break; 

        alarm (0); 
        write (STDOUT_FILENO, line, n); 
    } while (1); 
    exit (0); 
}
