#include "../apue.h" 
#include <pthread.h> 
#include <limits.h>

//#define STATIC_INIT
#define MUTEX_TYPE 2
#define TEST_TWICE_LOCK

#ifdef STATIC_INIT
#  if MUTEX_TYPE==0
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
#  elif MUTEX_TYPE==1
pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP; 
#  elif MUTEX_TYPE==2
pthread_mutex_t mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP; 
#  else
pthread_mutex_t mutex = PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP; 
#  endif
#else
pthread_mutex_t mutex; 
pthread_mutexattr_t attr; 
#endif

void* thr_fun (void *arg)
{
    int err = 0; 
    static int n = 0; 
    sleep (1); 
    printf ("this is turn %d\n", n++); 

    int type = 0; 
    pthread_mutex_lock (&mutex);
#ifdef TEST_TWICE_LOCK
    err = pthread_mutex_lock (&mutex); 
    if (err != 0)
        printf ("second lock failed, error %d\n", err); 
#endif

#ifndef STATIC_INIT
    err = pthread_mutexattr_gettype (&attr, &type); 
    if (err != 0)
        err_sys ("pthread_mutexattr_gettype failed\n"); 
    else 
        printf ("mutex type ? %d\n", type); 
#endif

    pthread_mutex_unlock (&mutex); 
#ifdef TEST_TWICE_LOCK
    err = pthread_mutex_unlock (&mutex); 
    if (err != 0)
        printf ("second unlock failed, error %d\n", err); 
#endif

    return NULL; //thr_fun (arg); 
}

int main ()
{
    int err = 0; 
    int type = 0; 
    pthread_t tid; 

#ifndef STATIC_INIT
    err = pthread_mutexattr_init (&attr); 
    if (err != 0)
        err_sys ("pthread_mutexattr_init failed\n"); 

    err = pthread_mutexattr_gettype (&attr, &type); 
    if (err != 0)
        err_sys ("pthread_mutexattr_gettype failed\n"); 
    else 
        printf ("mutex type ? %d\n", type); 

#  if MUTEX_TYPE == 0
    type = PTHREAD_MUTEX_NORMAL; 
    printf ("normal mutex\n"); 
#  elif MUTEX_TYPE == 1
    type = PTHREAD_MUTEX_RECURSIVE;
    printf ("recursive mutex\n"); 
#  elif MUTEX_TYPE == 2
    type = PTHREAD_MUTEX_ERRORCHECK; 
    printf ("error-check mutex\n"); 
#  else 
    type = PTHREAD_MUTEX_DEFAULT; 
    printf ("default mutex\n"); 
#  endif
    err = pthread_mutexattr_settype (&attr, type); 
    if (err != 0)
        err_sys ("pthread_mutexattr_settype failed\n"); 

    err = pthread_mutexattr_gettype (&attr, &type); 
    if (err != 0)
        err_sys ("pthread_mutexattr_gettype failed\n"); 
    else 
        printf ("mutex type ? %d\n", type); 

    err = pthread_mutex_init (&mutex, &attr); 
    if (err != 0)
        printf ("pthread_mutex_init failed\n"); 
#endif

    pthread_mutex_lock (&mutex); 
#ifdef TEST_TWICE_LOCK
    err = pthread_mutex_lock (&mutex); 
    if (err != 0)
        printf ("second lock failed, error %d\n", err); 
#endif
    err = pthread_create (&tid, NULL, thr_fun, NULL); 
    if (err != 0)
        err_sys ("pthread_create failed\n"); 

    sleep (5);
    pthread_mutex_unlock (&mutex); 
#ifdef TEST_TWICE_LOCK
    err = pthread_mutex_unlock (&mutex); 
    if (err != 0)
        printf ("second unlock failed, error %d\n", err); 
#endif

    printf ("prepare to join %lu\n", tid); 
    err = pthread_join (tid, NULL); 
    printf ("main thread exit\n"); 

#ifndef STATIC_INIT
    err = pthread_mutex_destroy (&mutex); 
    if (err != 0)
        printf ("pthread_mutex_destroy failed\n"); 

    err = pthread_mutexattr_destroy (&attr); 
    if (err != 0)
        err_sys ("pthread_mutexattr_destroy failed\n"); 
#endif

    return 0; 
}
