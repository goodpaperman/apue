#include "../apue.h"
#include <sys/wait.h>

static void sig_int (int signo)
{
    printf ("caught SIGINT\n"); 
}

static void sig_chld (int signo)
{
    printf ("caught SIGCHLD\n"); 

    int status = 0, ret = 0; 
    ret = waitpid (-1, &status, 0); 
    if (ret < 0)
        err_sys ("waitpid error"); 
    else 
        printf ("wait a child with status %d\n", status); 
}

int main ()
{
    if (apue_signal (SIGINT, sig_int) == SIG_ERR)
        err_sys ("signal (SIGINT) error"); 
    if (apue_signal (SIGCHLD, sig_chld) == SIG_ERR)
        err_sys ("signal (SIGCHLD) error"); 
    if (apue_system ("/bin/ed") < 0)
        err_sys ("system () error"); 

    //exit (0); 
    return 0; 
}
