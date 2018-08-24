#include "../apue.h" 
#include <sys/wait.h>
#include <errno.h>

static volatile sig_atomic_t sigflag; 
static sigset_t newmask, oldmask, zeromask; 

static void sig_usr (int signo)
{
    sigflag = 1; 
    printf ("SIGUSR1/2 called\n"); 
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

int read_increase_write ()
{
    char *ptr = NULL; 
    int n = 0, ret = 0, err = 0; 
    FILE* fp = fopen ("sync.txt", "w+"); 
    if (fp)
    {
        char buf[64] = { 0 }; 
        //fread (buf, sizeof(buf)-1, 1, fp); 
        do
        {
            errno = 0; 
            ptr = fgets (buf, sizeof(buf), fp); 
            err = errno; 
            printf ("read %p, errno %d\n", ptr, err); 
        } while (ptr == NULL && err == EINTR); 

        n = atoi (buf); 
        sprintf (buf, "%d", ++n); 
        ////printf ("write %s\n", buf); 
        //fwrite (buf, strlen (buf), 1, fp); 
        
        do 
        {
            errno = 0; 
            ret = fputs (buf, fp); 
            err = errno; 
            printf ("write %d, errno %d\n", ret, err); 
        } while (ret == 0 && err == EINTR); 
        printf ("[%d] read %d, write %d\n", getpid (), n-1, n); 
    }
    else 
    {
        printf ("open file failed\n"); 
        return -1; 
    }

    fclose (fp); 
    return n; 
}

int main (void)
{
    int ret = 0; 
    pid_t cid = 0; 
    //if (apue_signal (SIGUSR1, SIG_IGN) == SIG_ERR)
    //    err_sys ("signal (SIGUSR1) error"); 
    //if (apue_signal (SIGUSR2, SIG_IGN) == SIG_ERR)
    //    err_sys ("signal (SIGUSR2) error"); 

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
        //sleep (5); 
        while (ret < 10)
        {
            ret = read_increase_write (); 
            SYNC_TELL (getppid (), 0); 
            printf ("notify parent, prepare to wait\n"); 
            if (ret < 0 || ret >= 10)
                break; 

            SYNC_WAIT (); 
            printf ("wait parent %u\n", getppid ()); 
        }
        return 2; 
    }

    while (ret < 10 && ret >= 0)
    {
        printf ("prepare to wait child\n"); 
        SYNC_WAIT(); 
        printf ("wait child ready\n"); 
        ret = read_increase_write (); 
        SYNC_TELL (cid, 1); 
        printf ("notify chld\n"); 
    }

    pid_t wid = 0; 
    int status = 0; 
    //waitpid (cid, &status, 0); 
    if ((wid = wait (&status)) < 0)
        err_sys ("wait error"); 

    printf ("wait child %u exit %d\n", wid, WEXITSTATUS(status)); 
    return 1; 

}
