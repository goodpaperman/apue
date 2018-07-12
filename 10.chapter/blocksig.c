#include "../apue.h" 
#include <signal.h>

static void sig_int (int signo)
{
    printf ("caught SIGINT\n"); 
#if 0
    if (signal (SIGINT, SIG_DFL) == SIG_ERR)
#else 
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
#endif
        err_sys ("can't reset SIGINT"); 
}

int main (int argc, char *argv[])
{
    sigset_t newmask, oldmask, pendmask; 
#if 1
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
#else 
    if (signal (SIGINT, sig_int) == SIG_ERR)
#endif
        err_sys ("can't catch SIGINT"); 

    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGINT); 
    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys ("SIG_BLOCK error"); 

    sleep (5); 
    if (sigpending (&pendmask) < 0)
        err_sys ("sigpending error"); 
    if (sigismember (&pendmask, SIGINT))
        printf ("SIGINT pending\n"); 

#if 1 
    if (signal (SIGINT, sig_int) == SIG_ERR)
        err_sys ("can't catch SIGINT"); 
#endif

#if 1
    if (sigprocmask (SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys ("SIG_SETMASK error"); 
#else 
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
        err_sys ("can't ignore SIGINT"); 
#endif 

    printf ("SIGINT unblocked\n"); 
    sleep (5); 
    if (sigpending (&pendmask) < 0)
        err_sys ("sigpending error"); 
    if (sigismember (&pendmask, SIGINT))
        printf ("SIGINT pending\n"); 

    exit (0); 
}

