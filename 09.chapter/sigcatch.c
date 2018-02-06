#include <signal.h> 
#include <errno.h> 
#include "../apue.h" 

void sighandler (int signo)
{
    printf ("catch %d\n", signo); 
}

int main (int argc, char *argv[])
{
    int ret = 0; 
    signal (SIGINT, sighandler); 
    signal (SIGQUIT, sighandler); 
    while (1)
    {
        sleep (1); 
    }

    return 0; 
}
