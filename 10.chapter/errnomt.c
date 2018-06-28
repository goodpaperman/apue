#include  "../apue.h" 
#include <pthread.h> 
#include <errno.h> 
#include <signal.h> 

void sig_handler (int signum)
{
    printf ("signal %d received\n", signum); 
    errno = signum; 
}

void* thr_func (void *arg)
{
    while (1)
    {
        errno = 5; 
        sleep (3); 
        printf ("     errno = %d\n", errno); 
    }
}

int main ()
{
    pthread_t tid = 0; 
    int ret = pthread_create (&tid, NULL, thr_func, NULL); 
    if (ret != 0)
    {
        printf ("pthread create failed, errno %d\n", errno); 
        return -1; 
    }

    printf ("create thread %u\n", tid); 
    
    struct sigaction sigact; 
    sigact.sa_handler = sig_handler; 
    sigemptyset (&sigact.sa_mask); 
    sigact.sa_flags = SA_RESTART; 
    sigaction (SIGINT, &sigact, NULL); 

    while (1)
    {
        errno = 6; 
        sleep (2); 
        printf ("main errno = %d\n", errno); 
    }

    return 0; 
}
