#include "../apue.h" 
#include <signal.h>
#include <assert.h> 
#include <errno.h>

#define IGNORE2CATCH
#define USE_SUSPEND

static void sig_eater (int signo)
{
    printf ("caught %d\n", signo); 
    pr_procmask ("in sig_eater"); 
#if 0
    if (signal (signo, SIG_DFL) == SIG_ERR)
#elif 0 
    if (signal (signo, SIG_IGN) == SIG_ERR)
#else
    if (signal (signo, sig_eater) == SIG_ERR)
#endif
        err_sys ("can't reset %d", signo); 
}

int main (int argc, char *argv[])
{
    sigset_t newmask, oldmask, pendmask; 
#ifdef IGNORE2CATCH
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
#else 
    if (signal (SIGINT, sig_eater) == SIG_ERR)
#endif
        err_sys ("can't catch/ignore SIGINT"); 

#ifdef IGNORE2CATCH
    if (signal (SIGQUIT, SIG_IGN) == SIG_ERR)
#else 
    if (signal (SIGQUIT, sig_eater) == SIG_ERR)
#endif
        err_sys ("can't catch/ignore SIGQUIT"); 

    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGINT); 
    sigaddset (&newmask, SIGQUIT); 
    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys ("SIG_BLOCK error"); 

    pr_procmask ("after set mask"); 

#ifdef IGNORE2CATCH 
    if (signal (SIGINT, sig_eater) == SIG_ERR)
#else 
    if (signal (SIGINT, SIG_IGN) == SIG_ERR)
#endif
        err_sys ("can't catch/ignore SIGINT"); 

#ifdef IGNORE2CATCH 
    if (signal (SIGQUIT, sig_eater) == SIG_ERR)
#else 
    if (signal (SIGQUIT, SIG_IGN) == SIG_ERR)
#endif
        err_sys ("can't catch/ignore SIGQUIT"); 

    sleep (5); 

#if 0
    sigemptyset (&pendmask); 
    if (sigpending (&pendmask) < 0)
        err_sys ("sigpending error"); 
    if (sigismember (&pendmask, SIGINT))
        printf ("SIGINT pending\n"); 
    if (sigismember (&pendmask, SIGQUIT))
        printf ("SIGQUIT pending\n"); 
#else 
    pr_pendmask ("after got signal:"); 
    pr_procmask ("now proc mask: "); 
#endif 

#ifdef IGNORE2CATCH
#  ifdef USE_SUSPEND
    sigaddset (&oldmask, SIGABRT); 
    assert(sigsuspend (&oldmask) < 0); 
    assert(errno == EINTR); 
    printf ("SIGINT/QUIT unblocked\n"); 
#  else
    if (sigprocmask (SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys ("SIG_SETMASK error"); 

    printf ("SIGINT/QUIT unblocked\n"); 
    pause (); 
    //sleep (5); 
#  endif
#endif

#if 0
    if (sigpending (&pendmask) < 0)
        err_sys ("sigpending error"); 
    if (sigismember (&pendmask, SIGINT))
        printf ("SIGINT pending\n"); 
    if (sigismember (&pendmask, SIGQUIT))
        printf ("SIGQUIT pending\n"); 
#else 
    pr_pendmask ("now pend mask"); 
    pr_procmask ("now proc mask"); 
#endif

    exit (0); 
}

