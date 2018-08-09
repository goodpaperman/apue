#include "../apue.h" 
#include <setjmp.h> 
#include <time.h> 

#define USE_SIGJMP
static int g_signo = SIGINT; //SIGUSR1; 
static sigjmp_buf jmpbuf; 
static volatile sig_atomic_t canjump = 0; 

static void sig_eater (int signo)
{
    if (canjump == 0)
        return; 

    pr_procmask ("staring sig_eater:"); 
    abort (); 
    pr_procmask ("finishing sig_eater:"); 
    canjump = 0; 
}

static void sig_abrt (int signo)
{
    pr_procmask ("in sig_abrt: "); 
#ifdef USE_SIGJMP
    siglongjmp (jmpbuf, 1); 
#else 
    longjmp (jmpbuf, 1); 
#endif 
}

int main (void)
{
    if (apue_signal (g_signo, sig_eater) == SIG_ERR)
        err_sys ("signal (SIGINT) error"); 

    if (apue_signal (SIGABRT, sig_abrt) == SIG_ERR)
        err_sys ("signal (SIGABRT) error"); 

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

