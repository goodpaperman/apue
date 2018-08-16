#include "../apue.h"
#include <sys/wait.h>

#if 0
#  define SYSTEM system
#else
#  define SYSTEM apue_system
#endif 

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
        err_ret ("waitpid error"); 
    else 
        printf ("wait a child with status %d\n", status); 
}

int main (int argc, char *argv[])
{
    if (apue_signal (SIGINT, sig_int) == SIG_ERR)
        err_sys ("signal (SIGINT) error"); 
    if (apue_signal (SIGCHLD, sig_chld) == SIG_ERR)
        err_sys ("signal (SIGCHLD) error"); 
    int status = SYSTEM (argc > 1 ? argv[1] : "/bin/ed"); 
    if (status < 0)
        err_sys ("system () error"); 
    else 
        printf ("system return %d (0x%08x)\n", status, status); 

    pr_exit (status); 
    //exit (0); 
    return 0; 
}
