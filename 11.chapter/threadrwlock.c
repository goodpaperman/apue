#include "../apue.h" 
#include <pthread.h> 

#define PASSERT(ret) \
    if ((ret) != 0) \
        err_sys ("pthread api error"); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 

pthread_rwlock_t g_lock; 

void* thr_fn1 (void *arg)
{
    PASSERT(pthread_rwlock_rdlock (&g_lock)); 
    printf ("thr1 got read lock\n"); 
    sleep (5); 
    printf ("thr1 release read lock\n"); 
    PASSERT(pthread_rwlock_unlock (&g_lock)); 
    return NULL; 
}

void* thr_fn2 (void *arg)
{
    sleep (1); 
    PASSERT(pthread_rwlock_wrlock (&g_lock)); 
    printf ("thr2 got write lock\n"); 
    sleep (7); 
    printf ("thr2 release write lock\n"); 
    PASSERT(pthread_rwlock_unlock (&g_lock)); 
    return NULL; 
}

void* thr_fn3 (void *arg)
{
    sleep (2); 
    PASSERT(pthread_rwlock_rdlock (&g_lock)); 
    printf ("thr3 got read lock\n"); 
    sleep (9); 
    printf ("thr3 release read lock\n"); 
    PASSERT(pthread_rwlock_unlock (&g_lock)); 
    return NULL; 
}

int main (void)
{
    PASSERT(pthread_rwlock_init (&g_lock, NULL)); 

    pthread_t tid1, tid2, tid3; 
    PASSERT(pthread_create(&tid1, NULL, thr_fn1, NULL)); 
    PASSERT(pthread_create(&tid2, NULL, thr_fn2, NULL)); 
    PASSERT(pthread_create(&tid3, NULL, thr_fn3, NULL)); 
    
    sleep (1); 
    
    PASSERT(pthread_join (tid1, NULL)); 
    PASSERT(pthread_join (tid2, NULL)); 
    PASSERT(pthread_join (tid3, NULL)); 
    PASSERT(pthread_rwlock_destroy (&g_lock));
    return 0; 
}
