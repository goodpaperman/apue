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

int main (void)
{
    char c; 
    pid_t pid; 
    pr_ids ("parent"); 
    if ((pid = fork ()) < 0) { 
        err_sys ("fork error"); 
    } else if (pid > 0)
    {
        sleep (5); 
        exit (0); 
    } else {
        pr_ids ("child"); 
        signal (SIGHUP, sig_hup); 
        kill (getpid (), SIGTSTP); 
        pr_ids ("child"); 
        if (read (STDIN_FILENO, &c, 1) != 1)
            printf ("read error from controlling TTY, errno = %d\n", errno); 
        exit (0); 
    }
}
