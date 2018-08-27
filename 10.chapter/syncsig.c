#include "../apue.h" 
#include <sys/wait.h>
#include <errno.h>

#define USE_FGETS
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
#ifdef USE_FGETS
    FILE* fp = fopen ("sync.txt", "r"); 
    if (fp)
#else
    int fd = open ("sync.txt", O_CREAT | O_RDWR, 0622); 
    if (fd != -1)
#endif
    {
        char buf[64] = { 0 }; 
        //fread (buf, sizeof(buf)-1, 1, fp); 
        do
        {
            errno = 0; 
#ifdef USE_FGETS
            //ptr = fgets (buf, sizeof(buf), fp); 
            ret = fread (buf, 1, sizeof(buf), fp); 
        //    err = errno; 
        //    printf ("read %s, errno %d\n", ptr, err); 
        //} while (ptr == NULL && err == EINTR); 
#else 
            ret = read (fd, buf, sizeof(buf)); 
#endif
            err = errno; 
            printf ("read %d, errno %d\n", ret, err); 
        } while (ret == -1 && err == EINTR); 

        n = atoi (buf); 
        memset (buf, 0, sizeof(buf)); 
        sprintf (buf, "%d", ++n); 
        ////printf ("write %s\n", buf); 
        //fwrite (buf, strlen (buf), 1, fp); 
        
#ifdef USE_FGETS
        fp = freopen ("sync.txt", "w", fp); 
        if (!fp)
            err_sys ("freopen"); 
#endif
        
        do 
        {
            errno = 0; 
#ifdef USE_FGETS
            //if (fseek (fp, 0, SEEK_SET) == -1)
            //    err_sys ("fseek"); 

            //ret = fputs (buf, fp); 
            ret = fwrite (buf, 1, sizeof(buf), fp); 
#else 
            if (lseek (fd, 0, SEEK_SET) == -1)
                err_sys ("lseek"); 

            ret = write (fd, buf, sizeof(buf)); 
#endif
            err = errno; 
            printf ("write %d, errno %d\n", ret, err); 
        } while (ret == -1 && err == EINTR); 

        printf ("[%d] read %d, write %d\n", getpid (), n-1, n); 
    }
    else 
    {
        printf ("open file failed\n"); 
        return -1; 
    }

#ifdef USE_FGETS
    fclose (fp); 
#else
    close (fd); 
#endif
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
