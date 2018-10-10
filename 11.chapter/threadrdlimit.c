#include "../apue.h" 
#include <pthread.h> 

#define THR_MAX 600

#define PASSERT(ret) \
    if ((ret) != 0) \
        err_sys ("%s error", #ret); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 

pthread_rwlock_t g_lock; 

void* thr_fn1 (void *arg)
{
    PASSERT(pthread_rwlock_rdlock (&g_lock)); 
    printf ("[%lu] got read lock\n", pthread_self ()); 
    sleep (30); 
    printf ("[%lu] release read lock\n", pthread_self ()); 

    PASSERT(pthread_rwlock_unlock (&g_lock)); 
    return NULL; 
}

void* thr_fn2 (void *arg)
{
    sleep (1); 
    PASSERT(pthread_rwlock_wrlock (&g_lock)); 
    printf ("[%lu] got write lock\n", pthread_self ()); 
    sleep (10); 
    printf ("[%lu] release write lock\n", pthread_self ()); 

    PASSERT(pthread_rwlock_unlock (&g_lock)); 
    return NULL; 
}

int main (void)
{
    PASSERT(pthread_rwlock_init (&g_lock, NULL)); 
    int i=0; 
    pthread_t tid1[THR_MAX], tid2; 
    for (i=0; i<THR_MAX; ++ i)
    {
        PASSERT(pthread_create(&tid1[i], NULL, thr_fn1, NULL)); 
    }

    PASSERT(pthread_create(&tid2, NULL, thr_fn2, NULL)); 
    sleep (1); 
    
    for (i=0; i<THR_MAX; ++ i)
    {
        PASSERT(pthread_join (tid1[i], NULL)); 
    }

    PASSERT(pthread_join (tid2, NULL)); 
    PASSERT(pthread_rwlock_destroy (&g_lock));
    return 0; 
}
