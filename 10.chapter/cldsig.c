#include "../apue.h" 
#include <sys/wait.h> 

#define CLD_NUM 2
#define USE_SIG 1
#define USE_SIGACT 

#ifdef USE_SIGACT
#  define AUTO_WAIT
#endif

#ifdef USE_SIGACT
static void sig_cld (int signo, siginfo_t *info, void* param); 
#else
static void sig_cld (int); 
#endif

int main ()
{
    pid_t pid = 0; 
#if USE_SIG == 1
#  ifdef USE_SIGACT
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
    act.sa_sigaction = sig_cld; 
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP; 
#    ifdef AUTO_WAIT
    act.sa_flags |= SA_NOCLDWAIT; 
#    endif
    int ret = sigaction (SIGCHLD, &act, 0); 
    if (ret == -1)
        perror ("sigaction error"); 
#  else    
    __sighandler_t ret = signal (SIGCLD, sig_cld);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#  endif
#elif USE_SIG == 2
#  ifdef USE_SIGACT
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
    act.sa_sigaction = SIG_IGN; 
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP; 
#    ifdef AUTO_WAIT
    act.sa_flags |= SA_NOCLDWAIT; 
#    endif
    int ret = sigaction (SIGCHLD, &act, 0); 
    if (ret == -1)
        perror ("sigaction error"); 
#  else
    __sighandler_t ret = signal (SIGCLD, SIG_IGN);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#  endif
#endif

    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = fork ()) < 0)
            perror ("fork error"); 
        else if (pid == 0) 
        {
#if 1
            sleep (3); 
#else 
            sleep (1); 
            printf ("send me to background\n"); 
            kill(getpid (), SIGSTOP); 
#endif
            printf ("child %u exit\n", getpid ()); 
            _exit (0); 
        }

        sleep (1); 
    }

#if USE_SIG == 2
    int status = 0; 
    if ((pid = wait (&status)) < 0)
        perror ("wait error"); 

    printf ("pid = %d\n", pid); 
#elif USE_SIG == 3
    sleep (4); 
#  ifdef USE_SIGACT
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
    act.sa_sigaction = sig_cld; 
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
#    ifdef AUTO_WAIT
    act.sa_flags |= SA_NOCLDWAIT; 
#    endif
    int ret = sigaction (SIGCHLD, &act, 0); 
    if (ret == -1)
        perror ("sigaction error"); 
#  else
    __sighandler_t ret = signal (SIGCLD, sig_cld);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#  endif

    int status = 0; 
    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = wait (&status)) < 0)
            perror ("wait error"); 

        printf ("pid = %d\n", pid); 
    }
#elif USE_SIG == 4
    sleep (4); 
#  ifdef USE_SIGACT
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
    act.sa_sigaction = SIG_IGN; 
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP; 
#    ifdef AUTO_WAIT
    act.sa_flags |= SA_NOCLDWAIT; 
#    endif
    int ret = sigaction (SIGCHLD, &act, 0); 
    if (ret == -1)
        perror ("sigaction error"); 
#  else
    __sighandler_t ret = signal (SIGCLD, SIG_IGN);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#  endif

    int status = 0; 
    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = wait (&status)) < 0)
            perror ("wait error"); 

        printf ("pid = %d\n", pid); 
    }
#else
    for (int i=0; i<CLD_NUM; ++ i)
    {
        pause (); 
        printf ("wake up by signal %d\n", i); 
    }
#endif

    printf ("parent exit\n"); 
#ifdef AUTO_WAIT
    // pause to see if any defunct child leave...
    sleep (10); 
#endif
    return 0; 
}

#ifdef USE_SIGACT
static void sig_cld (int signo, siginfo_t *info, void* param)
{
    int status = 0; 
    if (signo == SIGCHLD)
    {
        if (info->si_code == CLD_EXITED ||
                info->si_code == CLD_KILLED || 
                info->si_code == CLD_DUMPED)
        {
#ifdef AUTO_WAIT
            printf ("pid (auto wait in signal) = %d\n", info->si_pid); 
#else 
            if (waitpid (info->si_pid, &status, 0) < 0)
                perror ("wait(in signal) error"); 
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
    printf ("pid (wait in signal) = %d\n", pid); 
}
#endif 
