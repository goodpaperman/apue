#include "../apue.h" 
#include <sys/wait.h>

static volatile sig_atomic_t sigflag; 
static sigset_t newmask, oldmask, zeromask; 

static void sig_usr (int signo)
{
    sigflag = 1; 
}

void SYNC_INIT (void)
{
    if (apue_signal (SIGUSR1, sig_usr) == SIG_ERR)
        err_sys ("signal (SIGUSR1) error"); 
    if (apue_signal (SIGUSR2, sig_usr) == SIG_ERR)
        err_sys ("signal (SIGUSR2) error"); 

    sigemptyset (&zeromask); 
    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGUSR1); 
    sigaddset (&newmask, SIGUSR2); 

    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys ("SIG_BLOCK error"); 
}

void SYNC_TELL (pid_t pid, int child)
{
    kill (pid, child ? SIGUSR1 : SIGUSR2); 
}

void SYNC_WAIT (void)
{
    while (sigflag == 0)
        sigsuspend (&zeromask); 

    sigflag = 0; 
    if (sigprocmask (SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys ("SIG_SETMASK error"); 
}

int main (void)
{
    pid_t cid = 0; 
    SYNC_INIT (); 
    cid = fork (); 
    if (cid < 0)
    {
        err_sys ("fork error"); 
        return -1; 
    }
    else if (cid == 0)
    {
        printf ("child %u running\n", getpid ()); 
        sleep (5); 
        SYNC_TELL (getppid (), 0); 
        printf ("notify parent, prepare to wait\n"); 
        SYNC_WAIT (); 
        printf ("wait parent %u\n", getppid ()); 
        return 2; 
    }

    printf ("prepare to wait child\n"); 
    SYNC_WAIT(); 
    printf ("wait child ready\n"); 
    SYNC_TELL (cid, 1); 
    printf ("notify chld\n"); 

    pid_t wid = 0; 
    int status = 0; 
    //waitpid (cid, &status, 0); 
    if ((wid = wait (&status)) < 0)
        err_sys ("wait error"); 

    printf ("wait child %u exit %d\n", wid, WEXITSTATUS(status)); 
    return 1; 

}
