#include "../apue.h" 
#include <errno.h> 

#define BUFSIZE 1024
#define SIGNAL signal // apue_signal

static void sig_tstp (int signo)
{
    sigset_t mask; 
    sigemptyset (&mask); 
    sigaddset (&mask, SIGTSTP); 
    sigprocmask (SIG_UNBLOCK, &mask, NULL); 
    SIGNAL (SIGTSTP, SIG_DFL); 
    kill (getpid (), SIGTSTP); 
    SIGNAL (SIGTSTP, sig_tstp); 
    printf ("reset tty mode\n"); 
    printf ("redraw screen\n"); 
}

int main (void)
{
    int n = 0; 
    char buf[BUFSIZE] = { 0 }; 
    if (SIGNAL (SIGTSTP, SIG_IGN) == SIG_DFL)
    {
        printf ("SIGTSTP use DFL handler, change to ours handler\n"); 
        SIGNAL (SIGTSTP, sig_tstp); 
    }

    while (n = read (STDIN_FILENO, buf, BUFSIZE))
    {
        if (n < 0)
        {
            if (errno == EINTR)
                continue; 
            else 
                break; 
        }

        if (write (STDOUT_FILENO, buf, n) != n)
            err_sys ("write error"); 
    }

    if (n < 0)
        err_sys ("read error"); 

    exit (0); 
}

