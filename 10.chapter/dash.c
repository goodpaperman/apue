#include "../apue.h" 
#include <sys/wait.h> 

#define CLD_NUM 2
#define USE_SIG 1
#define USE_SIGACT 

#if 0
#  define MY_SIG_CHILD SIGCLD
#else
#  define MY_SIG_CHILD SIGCHLD
#endif

#ifdef USE_SIGACT
//#  define AUTO_WAIT
#endif

#ifndef AUTO_WAIT
//#  define TEST_COMPETITION
#endif

#ifdef USE_SIGACT
static void sig_cld (int signo, siginfo_t *info, void* param); 
#else
static void sig_cld (int); 
#endif

#ifdef TEST_COMPETITION
void pid_remove (pid_t pid)
{
    printf ("remove pid %u\n", pid); 
}

void pid_add (pid_t pid)
{
    printf ("add pid %u\n", pid); 
}
#endif

void install_handler (__sighandler_t h)
{
#ifdef USE_SIGACT
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
#  ifdef AUTO_WAIT
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
#ifdef TEST_COMPETITION
        sigset_t mask; 
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL);
#endif

        if ((pid = fork ()) < 0)
            perror ("fork error"); 
        else if (pid == 0) 
        {
#ifdef TEST_COMPETITION
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
#else
            if (i % 2 == 0)
                sleep (3); 
            else {
                // to test stop
                sleep (1); 
                printf ("send me to background\n"); 
                kill(getpid (), SIGSTOP); 
            }
#endif
            printf ("child %u exit\n", getpid ()); 
            _exit (0); 
        }

        sleep (1); 
#ifdef TEST_COMPETITION
        pid_add (pid); 
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
#endif
    }

#if USE_SIG == 1
#  ifdef AUTO_WAIT
    // pause to see if any defunct child leave...
    sleep (10); 
#  elif defined(TEST_COMPETITION)
    sleep (1); 
#  else
    for (int i=0; i<CLD_NUM; ++ i)
    {
        pause (); 
        printf ("wake up by signal %d\n", i); 
    }
#  endif
#elif USE_SIG == 2
    int status = 0; 
    if ((pid = wait (&status)) < 0)
        perror ("wait error"); 

    printf ("pid = %d\n", pid); 
#elif USE_SIG == 3
    sleep (4); 
    install_handler (sig_cld); 

    int status = 0; 
    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = wait (&status)) < 0)
            perror ("wait error"); 

        printf ("pid = %d\n", pid); 
    }
#elif USE_SIG == 4
    sleep (4); 
    install_handler (SIG_IGN); 

    int status = 0; 
    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = wait (&status)) < 0)
            perror ("wait error"); 

        printf ("pid = %d\n", pid); 
    }
#endif

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
                perror ("wait(in signal) error"); 
            printf ("pid (wait in signal) = %d\n", info->si_pid); 
#endif

#ifdef TEST_COMPETITION
            pid_remove (info->si_pid); 
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

#ifdef TEST_COMPETITION
    pid_remove (pid); 
#endif
}
#endif 
