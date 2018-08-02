#include "../apue.h" 
#include <signal.h>
#include <assert.h> 
#include <errno.h>

#define IGNORE2CATCH
#define USE_SUSPEND

static void sig_int (int signo)
{
    //printf ("caught SIGINT\n"); 
    pr_procmask ("in sig_int"); 
#if 0
    if (signal (SIGINT, SIG_DFL) == SIG_ERR)
#elif 0 
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
#else
    if (signal (SIGINT, sig_int) == SIG_ERR)
#endif
        err_sys ("can't reset SIGINT"); 
}

int main (int argc, char *argv[])
{
    sigset_t newmask, oldmask, pendmask; 
#ifdef IGNORE2CATCH
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
#else 
    if (signal (SIGINT, sig_int) == SIG_ERR)
#endif
        err_sys ("can't catch/ignore SIGINT"); 

    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGINT); 
    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys ("SIG_BLOCK error"); 

    pr_procmask ("after set mask"); 

#ifdef IGNORE2CATCH 
    if (signal (SIGINT, sig_int) == SIG_ERR)
#else 
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
#endif
        err_sys ("can't catch/ignore SIGINT"); 

    sleep (5); 

#if 0
    sigemptyset (&pendmask); 
    if (sigpending (&pendmask) < 0)
        err_sys ("sigpending error"); 
    if (sigismember (&pendmask, SIGINT))
        printf ("SIGINT pending\n"); 
#else 
    pr_pendmask ("after got signal:"); 
    pr_procmask ("now proc mask: "); 
#endif 

#ifdef IGNORE2CATCH
#  ifdef USE_SUSPEND
    assert(sigsuspend (&oldmask) < 0); 
    assert(errno == EINTR); 
    printf ("SIGINT unblocked\n"); 
#  else
    if (sigprocmask (SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys ("SIG_SETMASK error"); 

    printf ("SIGINT unblocked\n"); 
    pause (); 
    //sleep (5); 
#  endif
#endif

#if 0
    if (sigpending (&pendmask) < 0)
        err_sys ("sigpending error"); 
    if (sigismember (&pendmask, SIGINT))
        printf ("SIGINT pending\n"); 
#else 
    pr_pendmask ("after set mask"); 
#endif

    exit (0); 
}

