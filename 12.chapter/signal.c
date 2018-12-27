#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

#define THR_MAX 2

#define PASSERT(ret) \
{\
    int __ret=(ret);\
    if (__ret != 0) {\
        printf ("pthread api error %s: %d\n", #ret, __ret); \
        exit (__ret); \
    }\
    else \
        printf ("[%lu %lu] %s\n", pthread_self(),  #ret); \
}

int quitflag; 
sigset_t mask; 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t wait = PTHREAD_COND_INITIALIZER; 

void sig_handler (int signo)
{
    printf ("caught signal %d\n", signo); 
    signal (signo, sig_handler); 
    if (signo == SIGQUIT)
        quitflag = 1; 
}

void* thr_fun (void *arg)
{
    int n = (int)arg; 
    char msg[128] = { 0 }; 
    sprintf (msg, "%lu", pthread_self ()); 
    pr_threadmask (msg); 
    if (n == 0)
    {
        int i; 
        for (i=1; i<_NSIG; ++ i)
        {
            if (sigismember(&mask, i))
            {
#if 0
                signal (i, sig_handler); 
#else
                struct sigaction act; 
                act.sa_handler = sig_handler; 
                sigemptyset (&act.sa_mask); 
                act.sa_flags = 0; 
                sigaction (i, &act, NULL); 
#endif
                printf ("setup handler for signal %d\n", i); 
            }
        }

#if 1 // unblock the inherit signal mask
        sigset_t non; 
        sigemptyset (&non); 
        PASSERT(pthread_sigmask (SIG_SETMASK, &non, NULL)); 
#endif 
        while (quitflag == 0)
            sleep (10); 

        printf ("thread prepare to exit, notify main!\n"); 
        PASSERT(pthread_cond_signal (&wait)); 
    }
    else 
    {
        int signo; 
        for (;;) { 
            PASSERT(sigwait (&mask, &signo)); 
            switch (signo)
            {
                case SIGINT:
                    printf ("\ninterrupt\n"); 
                    break; 

                case SIGQUIT:
                    PASSERT(pthread_mutex_lock (&lock)); 
                    quitflag = 1; 
                    PASSERT(pthread_mutex_unlock (&lock)); 
                    PASSERT(pthread_cond_signal (&wait)); 
                    return 0; 

                default:
                    printf ("unexpected signal %d\n", signo); 
                    exit (1); 
            }
        }
    }

    return NULL; 
}

int main (int argc, char *argv[])
{
    int i; 
    sigset_t oldmask; 
    pthread_t tid[THR_MAX]; 

    sigemptyset (&mask); 
    sigaddset (&mask, SIGINT); 
    sigaddset (&mask, SIGQUIT); 
    PASSERT(pthread_sigmask (SIG_BLOCK, &mask, &oldmask)); 
    // note new thread will inherit the blocked signal mask !
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, (void*)i)); 

    PASSERT(pthread_mutex_lock (&lock)); 
    while (quitflag == 0)
        PASSERT(pthread_cond_wait (&wait, &lock)); 

    PASSERT(pthread_mutex_unlock (&lock)); 
    
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    quitflag = 0; 
    PASSERT(sigprocmask (SIG_SETMASK, &oldmask, NULL)); 
    // to see signal handler changed by thread
    sleep (10); 
    return 0; 
}

