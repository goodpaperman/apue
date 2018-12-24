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
        printf ("[%lu] %s\n", pthread_self(),  #ret); \
}

int quitflag; 
sigset_t mask; 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t wait = PTHREAD_COND_INITIALIZER; 

void* thr_fun (void *arg)
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
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, 0)); 

    PASSERT(pthread_mutex_lock (&lock)); 
    while (quitflag == 0)
        PASSERT(pthread_cond_wait (&wait, &lock)); 

    PASSERT(pthread_mutex_unlock (&lock)); 
    
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    quitflag = 0; 
    PASSERT(sigprocmask (SIG_SETMASK, &oldmask, NULL)); 
    return 0; 
}

