#include "../apue.h" 
#include <pthread.h> 
#include <errno.h> 

#define CROSS_LOCK

struct foo {
    int f_count; 
    pthread_mutex_t f_lock; 
}; 

struct foo* foo_alloc (void)
{
    struct foo* fp; 
    if ((fp = malloc (sizeof (struct foo))) != NULL) { 
        fp->f_count = 1; 
        printf ("thread %lu create me, ref count %d\n", pthread_self (), fp->f_count); 
        if (pthread_mutex_init (&fp->f_lock, NULL) != 0) { 
            free (fp); 
            return NULL; 
        }
    }

    return fp; 
}

void foo_hold (struct foo *fp)
{
    pthread_mutex_lock (&fp->f_lock); 
    fp->f_count ++; 
    printf ("thread %lu hold me, ref count %d\n", pthread_self (), fp->f_count); 
    pthread_mutex_unlock (&fp->f_lock); 
}

void foo_rele (struct foo *fp)
{
    pthread_mutex_lock (&fp->f_lock); 
    if (--fp->f_count == 0) { 
        printf ("thread %lu destroy me, ref count %d\n", pthread_self (), fp->f_count); 
        pthread_mutex_unlock (&fp->f_lock); 
        pthread_mutex_destroy (&fp->f_lock); 
        free (fp); 
    }
    else {
        printf ("thread %lu release me, ref count %d\n", pthread_self (), fp->f_count); 
        pthread_mutex_unlock (&fp->f_lock); 
    }
}

void* thr_fn (void *arg)
{
    printf ("thread %lu running\n", pthread_self ()); 
    struct foo* fp = (struct foo*) arg; 
    foo_hold (fp); 
    sleep (1); 
    foo_rele (fp); 
    printf ("thread %lu exit\n", pthread_self ()); 
    return NULL; 
}

int main ()
{
    struct foo* fp = foo_alloc (); 
    if (fp == NULL)
        return -1; 

    foo_hold (fp); 

    int ret = 0; 
    pthread_t tid; 
    ret = pthread_create (&tid, NULL, thr_fn, fp); 
    if (ret != 0)
        err_quit ("create thread failed, errno %d\n", errno); 

#ifndef CROSS_LOCK
    foo_rele (fp); 
#endif 

    ret = pthread_join (tid, NULL); 
    if (ret != 0)
        err_quit ("join thread failed, errno %d\n", errno); 

    printf ("thread %lu joined\n", tid); 

#ifdef CROSS_LOCK
    foo_rele (fp); 
#endif 
    foo_rele (fp); 

    return 0; 
}

