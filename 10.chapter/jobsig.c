#include "../apue.h" 

static void sig_int (int signo)
{
    printf ("SIGINT caught\n"); 
    pr_pendmask ("in sigint"); 
    signal (SIGINT, sig_int); 
}

static void sig_tstp (int signo)
{
    printf ("SIGTSTP caught\n"); 
    kill (getpid (), SIGTSTP); 
    signal (SIGTSTP, sig_tstp); 
}

static void sig_cont (int signo)
{
    printf ("SIGCONT caught\n"); 
    signal (SIGCONT, sig_cont); 
}

int main (void)
{
    if (signal (SIGINT, sig_int) == SIG_ERR)
        err_sys ("signal(SIGINT) failed"); 
    if (signal (SIGTSTP, sig_tstp) == SIG_ERR)
        err_sys ("signal(SIGTSTP) failed"); 
#if 0
    if (signal (SIGCONT, SIG_IGN) == SIG_ERR)
        err_sys ("signal (SIGCONT) failed"); 
#else 
    if (signal (SIGCONT, sig_cont) == SIG_ERR)
        err_sys ("signal(SIGCONT) failed"); 

    sigset_t mask, old; 
    sigemptyset (&mask); 
    sigaddset (&mask, SIGTSTP); 
    //sigaddset (&mask, SIGCONT); 
    sigprocmask (SIG_BLOCK, &mask, &old); 
#endif

    while (1)
        pause (); 

    //sigprocmask (SIG_SETMASK, &old, NULL); 
    return 0; 
}
