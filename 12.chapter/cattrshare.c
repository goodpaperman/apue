#include "../apue.h" 
#include <pthread.h> 

#define THR_MAX 3

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
    int shared = 0; 
    pthread_mutex_t lock; 
    pthread_mutexattr_t lattr; 
    pthread_cond_t cond; 
    pthread_condattr_t cattr; 

    PASSERT(pthread_mutexattr_init(&lattr)); 
    PASSERT(pthread_mutexattr_getpshared(&lattr, &shared)); 
    printf ("default lock shared: %d\n", shared); 
    PASSERT(pthread_mutexattr_setpshared(&lattr, PTHREAD_PROCESS_SHARED)); 
    PASSERT(pthread_mutexattr_getpshared(&lattr, &shared)); 
    printf ("set lock shared: %d\n", shared); 
    PASSERT(pthread_mutex_init (&lock, &lattr)); 
    PASSERT(pthread_mutexattr_destroy (&lattr)); 

    PASSERT(pthread_condattr_init (&cattr)); 
    PASSERT(pthread_condattr_getpshared(&cattr, &shared)); 
    printf ("default condition shared: %d\n", shared); 
    PASSERT(pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED)); 
    PASSERT(pthread_condattr_getpshared(&cattr, &shared)); 
    printf ("set condition shared: %d\n", shared); 
    PASSERT(pthread_cond_init (&cond, &cattr)); 
    PASSERT(pthread_condattr_destroy (&cattr)); 

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

    PASSERT(pthread_cond_destroy (g_cond)); 
    PASSERT(pthread_mutex_destroy (g_lock)); 
    return 0; 
}

