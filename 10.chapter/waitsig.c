#include "../apue.h" 

volatile sig_atomic_t quitflag = 0; 

static void sig_int (int signo)
{
    if (signo == SIGINT)
        printf ("\ninterrupt\n"); 
    else if (signo == SIGQUIT)
    {
        printf ("\nquiting...\n"); 
        quitflag = 1; 
    }
}

int main (void)
{
    sigset_t newmask, oldmask, zeromask; 
    if (apue_signal (SIGINT, sig_int) == SIG_ERR)
        err_sys ("signal(SIGINT) error"); 
    if (apue_signal (SIGQUIT, sig_int) == SIG_ERR)
        err_sys ("signal(SIGQUIT) error"); 

    sigemptyset (&zeromask); 
    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGQUIT); 
    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys ("SIG_BLOCK error"); 

    while (quitflag == 0)
    {
#if 1
        sigsuspend (&zeromask); 
#else
        sigprocmask (SIG_SETMASK, &oldmask, NULL); 
        pause (); 
        sigprocmask (SIG_BLOCK, &newmask, &oldmask); 
#endif
    }

    quitflag = 0; 
    if (sigprocmask (SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys ("SIG_SETMASK error"); 

    exit (0); 
}

