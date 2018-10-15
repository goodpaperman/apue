#include "../apue.h" 
#include <pthread.h> 

#define THR_MAX 3
//#define STATIC_INIT

#define PASSERT(ret) \
    if ((ret) != 0) \
        err_sys ("pthread api error"); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 

pthread_mutex_t *g_lock;
pthread_cond_t *g_cond; 

void* thr_fn (void *arg)
{
    int *pi = (int *) arg; 
    PASSERT(pthread_mutex_lock (g_lock)); 
    printf ("thr %d got lock\n", *pi); 
    PASSERT(pthread_cond_wait (g_cond, g_lock)); 
    printf ("thr %d release lock\n", *pi); 
    PASSERT(pthread_mutex_unlock (g_lock)); 
    free (pi); 
    return NULL; 
}

int main (void)
{
#ifdef STATIC_INIT
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
#else
    pthread_mutex_t lock; 
    pthread_cond_t cond; 
    PASSERT(pthread_mutex_init (&lock, NULL)); 
    PASSERT(pthread_cond_init (&cond, NULL)); 
#endif

    int i = 0; 
    g_lock = &lock; 
    g_cond = &cond; 
    pthread_t tid[THR_MAX]; 
    for (i=0; i<THR_MAX; ++ i)
    {
        int *pi = (int *) malloc (sizeof (int)); 
        *pi = i; 
        PASSERT(pthread_create(&tid[i], NULL, thr_fn, (void *)pi)); 
    }
    
    sleep (1); 
    for (i=0; i<THR_MAX; ++i)
    {
        PASSERT(pthread_cond_signal (g_cond)); 
    }
    
    for (i=0; i<THR_MAX; ++ i)
    {
        PASSERT(pthread_join (tid[i], NULL)); 
    }

#ifndef STATIC_INIT
    PASSERT(pthread_cond_destroy (g_cond)); 
    PASSERT(pthread_mutex_destroy (g_lock)); 
#endif

    return 0; 
}
