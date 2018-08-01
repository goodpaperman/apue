#include "../apue.h" 
#include <setjmp.h> 
#include <time.h> 

#define USE_SIGJMP
static int g_signo = SIGINT; //SIGUSR1; 
static sigjmp_buf jmpbuf; 
static volatile sig_atomic_t canjump = 0; 

static void sig_eater (int signo)
{
    time_t starttime; 
    if (canjump == 0)
        return; 

    pr_procmask ("staring sig_eater:"); 
    alarm (3); 
    starttime = time (NULL); 
    while (1)
        if (time(NULL) > starttime + 5)
            break; 

    pr_procmask ("finishing sig_eater:"); 
    canjump = 0; 
#ifdef USE_SIGJMP
    siglongjmp (jmpbuf, 1); 
#else 
    longjmp (jmpbuf, 1); 
#endif 
}

static void sig_alrm (int signo)
{
    pr_procmask ("in sig_alrm: "); 
}

int main (void)
{
    if (apue_signal (g_signo, sig_eater) == SIG_ERR)
        err_sys ("signal (SIGUSR1) error"); 

    if (apue_signal (SIGALRM, sig_alrm) == SIG_ERR)
        err_sys ("signal (SIGALRM) error"); 

    pr_procmask ("starting main: "); 
#ifdef USE_SIGJMP
    if (sigsetjmp (jmpbuf, 1))
#else
    if (setjmp (jmpbuf))
#endif
    {
        pr_procmask ("ending main: "); 
        exit (0); 
    }

    canjump = 1; 
    while (1)
        pause (); 
}

