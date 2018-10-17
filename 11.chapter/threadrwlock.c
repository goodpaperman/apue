#include "../apue.h" 
#include <pthread.h> 

#define TEST_RWLOCK
#define STATIC_INIT

#define PASSERT(ret) \
    if ((ret) != 0) \
        err_sys ("pthread api error"); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 

#ifdef TEST_RWLOCK
pthread_rwlock_t g_lock
#  ifdef STATIC_INIT
    = PTHREAD_RWLOCK_INITIALIZER; 
#  else
    ;
#  endif
#else 
pthread_mutex_t g_lock
#  ifdef STATIC_INIT
    = PTHREAD_MUTEX_INITIALIZER; 
#  else
    ;
#  endif
#endif 

void* thr_fn1 (void *arg)
{
#ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_rdlock (&g_lock)); 
#else 
    PASSERT(pthread_mutex_lock (&g_lock)); 
#endif

    printf ("thr1 got read lock\n"); 
    sleep (5); 
    printf ("thr1 release read lock\n"); 

#ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_unlock (&g_lock)); 
#else 
    PASSERT(pthread_mutex_unlock (&g_lock)); 
#endif 

    return NULL; 
}

void* thr_fn2 (void *arg)
{
    sleep (1); 
#ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_wrlock (&g_lock)); 
#else
    PASSERT(pthread_mutex_lock (&g_lock)); 
#endif 

    printf ("thr2 got write lock\n"); 
    sleep (7); 
    printf ("thr2 release write lock\n"); 

#ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_unlock (&g_lock)); 
#else
    PASSERT(pthread_mutex_unlock (&g_lock)); 
#endif 

    return NULL; 
}

void* thr_fn3 (void *arg)
{
    sleep (2); 
#ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_rdlock (&g_lock)); 
#else 
    PASSERT(pthread_mutex_lock (&g_lock)); 
#endif

    printf ("thr3 got read lock\n"); 
    sleep (9); 
    printf ("thr3 release read lock\n"); 

#ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_unlock (&g_lock)); 
#else 
    PASSERT(pthread_mutex_unlock (&g_lock));
#endif 

    return NULL; 
}

int main (void)
{
#ifndef STATIC_INIT
#  ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_init (&lock, NULL)); 
#  else 
    PASSERT(pthread_mutex_init (&lock, NULL)); 
#  endif
#endif 

    //g_lock = &lock; 
    pthread_t tid1, tid2, tid3; 
    PASSERT(pthread_create(&tid1, NULL, thr_fn1, NULL)); 
    PASSERT(pthread_create(&tid2, NULL, thr_fn2, NULL)); 
    PASSERT(pthread_create(&tid3, NULL, thr_fn3, NULL)); 
    
    sleep (1); 
    
    PASSERT(pthread_join (tid1, NULL)); 
    PASSERT(pthread_join (tid2, NULL)); 
    PASSERT(pthread_join (tid3, NULL)); 

#ifndef STATIC_INIT
#  ifdef TEST_RWLOCK
    PASSERT(pthread_rwlock_destroy (&g_lock));
#  else 
    PASSERT(pthread_mutex_destroy (&g_lock)); 
#  endif
#endif

    return 0; 
}
