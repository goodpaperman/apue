#include "../apue.h" 

static void sig_int (int signo)
{
    pr_procmask ("\nin sig_int:"); 
}

void main ()
{
    sigset_t newmask, oldmask, waitmask; 
    pr_procmask ("program start: "); 
    if (apue_signal (SIGINT, sig_int) == SIG_ERR)
        err_sys ("signal (SIGINT) error"); 

    sigemptyset (&waitmask); 
    sigaddset (&waitmask, SIGUSR1); 
    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGINT); 

    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys ("SIG_BLOCK error"); 

    pr_procmask ("in critical region which block SIGINT:"); 
    if (sigsuspend (&waitmask) != -1)
        err_sys ("sigsuspend error"); 

    pr_procmask ("after return from sigsuspend:"); 
    if (sigprocmask (SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys ("SIG_SETMASK error"); 

    pr_procmask ("program exit:"); 
    exit (0); 
}
