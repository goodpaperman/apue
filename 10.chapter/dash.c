#include "../apue.h" 
#include <sys/wait.h> 
#include <errno.h> 

#define CLD_NUM 2
#define USE_SIG 1
#define USE_SIGACT 
#define USE_MASK

#if 0
#  define MY_SIG_CHILD SIGCLD
#else
#  define MY_SIG_CHILD SIGCHLD
#endif

#ifdef USE_SIGACT
//#  define AUTO_WAIT
#endif

#ifdef AUTO_WAIT
//#  define AUTO_WAIT_DFL
#endif

#ifdef USE_SIGACT
static void sig_cld (int signo, siginfo_t *info, void* param); 
#else
static void sig_cld (int); 
#endif

void install_handler (__sighandler_t h)
{
#ifdef USE_SIGACT
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
#  ifdef AUTO_WAIT_DFL
    act.sa_sigaction = SIG_DFL; 
#  else 
    act.sa_sigaction = h; 
#  endif
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP; 
#  ifdef AUTO_WAIT
    act.sa_flags |= SA_NOCLDWAIT; 
#  endif
    int ret = sigaction (MY_SIG_CHILD, &act, 0); 
    if (ret == -1)
        perror ("sigaction error"); 
#else    
    __sighandler_t ret = signal (SIGCLD, h);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#endif
}


int main ()
{
    pid_t pid = 0; 
#if USE_SIG == 1
    install_handler (sig_cld); 
#elif USE_SIG == 2
    install_handler (SIG_IGN); 
#endif

    for (int i=0; i<CLD_NUM; ++ i)
    {
#ifdef USE_MASK
        sigset_t mask; 
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL);
#endif 
        if ((pid = fork ()) < 0)
            perror ("fork error"); 
        else if (pid == 0) 
        {
#ifdef USE_MASK
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
#endif
            if (i % 2 == 0) { 
                // simulate background
                sleep (2); 
            }
            else {
                // simulate foreground
                sleep (3); 
            }

            printf ("child %u exit\n", getpid ()); 
            _exit (0); 
        }

#ifdef USE_MASK
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
#endif
        sleep (1); 
    }

    int status = 0; 
    while (1) { 
        if (waitpid (pid, &status, 0) < 0)
        {
            int err = errno; 
            printf ("wait %u error %d\n", pid, err); 
            if (err == EINTR)
                continue; 
        }
        else
            printf ("wait child pid = %d\n", pid); 

        break; 
    }

    sleep (2);
    printf ("parent exit\n"); 
    return 0; 
}

#ifdef USE_SIGACT
static void sig_cld (int signo, siginfo_t *info, void* param)
{
    int status = 0; 
    if (signo == MY_SIG_CHILD)
    {
        if (info->si_code == CLD_EXITED ||
                info->si_code == CLD_KILLED || 
                info->si_code == CLD_DUMPED)
        {
#ifdef AUTO_WAIT
            printf ("pid (auto wait in signal) = %d\n", info->si_pid); 
#else 
            if (waitpid (info->si_pid, &status, 0) < 0)
                err_ret ("wait(in signal) %u error", info->si_pid); 
            else 
                printf ("pid (wait in signal) = %d\n", info->si_pid); 
#endif
        }
        else 
        {
            printf ("unknown signal code %d\n", info->si_code); 
        }
    }
}
#else
static void sig_cld (int signo)
{
    pid_t pid = 0; 
    int status = 0; 
    printf ("SIGCLD received\n"); 
#if 1
    if (signal (SIGCLD, sig_cld) == SIG_ERR)
        perror ("signal error"); 
#endif

    if ((pid = wait (&status)) < 0)
        perror ("wait(in signal) error"); 
    else
        printf ("pid (wait in signal) = %d\n", pid); 
}
#endif 
