#include "../apue.h" 
#include <setjmp.h> 
#include <time.h> 

#define USE_SIGJMP 2
static int g_signo = SIGINT; //SIGUSR1; 
static sigjmp_buf jmpbuf; 
static volatile sig_atomic_t canjump = 0; 

static void sig_eater (int signo)
{
    if (canjump == 0)
        return; 

    pr_procmask ("staring sig_eater:"); 
#if 0
    abort (); 
#else 
    apue_abort (); 
#endif 
    pr_procmask ("finishing sig_eater:"); 
    canjump = 0; 
}

static void sig_abrt (int signo)
{
    pr_procmask ("in sig_abrt: "); 
#if USE_SIGJMP==2
    siglongjmp (jmpbuf, 1); 
#elif USE_SIGJMP==1 
    longjmp (jmpbuf, 1); 
#else 
#endif 
}

int main (void)
{
    if (apue_signal (g_signo, sig_eater) == SIG_ERR)
        err_sys ("signal (SIGINT) error"); 

#if 1
    if (apue_signal (SIGABRT, sig_abrt) == SIG_ERR)
#else 
    if (apue_signal (SIGABRT, SIG_IGN) == SIG_ERR)
#endif
        err_sys ("signal (SIGABRT) error"); 

    pr_procmask ("starting main: "); 
#if USE_SIGJMP==2
    if (sigsetjmp (jmpbuf, 1))
#elif USE_SIGJMP==1
    if (setjmp (jmpbuf))
#else
    if (0)
#endif
    {
        pr_procmask ("ending main: "); 
        exit (0); 
    }

    canjump = 1; 
    while (1)
        pause (); 
}

