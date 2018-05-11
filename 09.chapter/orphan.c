#include "../apue.h" 
#include <errno.h> 

static void sig_hup (int signo)
{
    printf ("SIGHUP received, pid = %d\n", getpid ()); 
}

static void pr_ids (char const* name)
{
    printf ("%s: pid = %d, ppid = %d, pgrp = %d, tpgrp = %d, sess = %d\n", 
            name, getpid (), getppid (), getpgrp (), tcgetpgrp (STDIN_FILENO), getsid (0)); 
    fflush (stdout); 
}

#define N 5
#define ALL_STOP
//#define ALL_SLEEP

int main (void)
{
    char c; 
    pid_t pid; 
    pr_ids ("parent"); 
    for (int i=0; i<N; ++ i)
    {
        if ((pid = fork ()) < 0) { 
            err_sys ("fork error"); 
        } else if (pid == 0) {
            char name[128] = { 0 }; 
            sprintf (name, "%s%d", "child", i+1); 
            pr_ids (name); 
            signal (SIGHUP, sig_hup); 
            if (i%2 == 0)
            {
#ifdef ALL_STOP
                printf ("before stop self: %d\n", getpid ()); 
                kill (getpid (), SIGTSTP); 
#else 
                printf ("before sleep self: %d\n", getpid ()); 
                sleep (6); 
#endif
            }
            else 
            {
#ifdef ALL_SLEEP
                printf ("before sleep self: %d\n", getpid ()); 
                sleep (6); 
#else
                printf ("before stop self: %d\n", getpid ()); 
                kill (getpid (), SIGTSTP); 
#endif 
            }

            pr_ids (name); 
            if (read (STDIN_FILENO, &c, 1) != 1)
                printf ("read error from controlling TTY, errno = %d\n", errno); 

            exit (0); 
        }
    }

    sleep (5); 
    exit (0); 
    return 0; 
}
