#include "../apue.h" 
#include <pthread.h> 
#include <limits.h>

pthread_mutex_t mutex; 
pthread_mutexattr_t attr; 

void* thr_fun (void *arg)
{
    static int n = 0; 
    sleep (1); 
    printf ("this is turn %d\n", n++); 

    int share = 0; 
    pthread_mutex_lock (&mutex);
    int err = pthread_mutexattr_getpshared (&attr, &share); 
    if (err != 0)
        err_sys ("pthread_mutexattr_getpshared failed\n"); 
    else 
        printf ("mutex share ? %d\n", share); 
    pthread_mutex_unlock (&mutex); 

    return NULL; //thr_fun (arg); 
}

int main ()
{
    int err = 0; 
    int share = 0; 
    pthread_t tid; 

    err = pthread_mutexattr_init (&attr); 
    if (err != 0)
        err_sys ("pthread_mutexattr_init failed\n"); 

    err = pthread_mutexattr_getpshared (&attr, &share); 
    if (err != 0)
        err_sys ("pthread_mutexattr_getpshared failed\n"); 
    else 
        printf ("mutex share ? %d\n", share); 

#if 0
    share = 0; 
#else
    share = 1; 
#endif
    err = pthread_mutexattr_setpshared (&attr, share); 
    if (err != 0)
        err_sys ("pthread_mutexattr_setpshared failed\n"); 

    err = pthread_mutexattr_getpshared (&attr, &share); 
    if (err != 0)
        err_sys ("pthread_mutexattr_getpshared failed\n"); 
    else 
        printf ("mutex share ? %d\n", share); 

    err = pthread_mutex_init (&mutex, &attr); 
    if (err != 0)
        printf ("pthread_mutex_init failed\n"); 

    pthread_mutex_lock (&mutex); 
    err = pthread_create (&tid, NULL, thr_fun, NULL); 
    if (err != 0)
        err_sys ("pthread_create failed\n"); 

    sleep (5);
    pthread_mutex_unlock (&mutex); 
    printf ("prepare to join %lu\n", tid); 
    err = pthread_join (tid, NULL); 
    printf ("main thread exit\n"); 

    err = pthread_mutex_destroy (&mutex); 
    if (err != 0)
        printf ("pthread_mutex_destroy failed\n"); 

    err = pthread_mutexattr_destroy (&attr); 
    if (err != 0)
        err_sys ("pthread_mutexattr_destroy failed\n"); 

    return 0; 
}
