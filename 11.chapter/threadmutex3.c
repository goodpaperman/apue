#include "../apue.h" 
#include <pthread.h> 

#define NHASH 29
#define HASH(p) (((unsigned long)p)%NHASH)

struct foo *fh[NHASH]; 
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER; 

struct foo {
    int f_count; 
    pthread_mutex_t f_lock; 
    struct foo *f_next; 
    int f_id; 
}; 

struct foo* foo_alloc (int id)
{
    struct foo* fp; 
    int idx; 
    if ((fp = malloc (sizeof (struct foo))) != NULL) { 
        fp->f_count = 1; 
        printf ("thread %lu create me, ref count %d\n", pthread_self (), fp->f_count); 
        if (pthread_mutex_init (&fp->f_lock, NULL) != 0) { 
            free (fp); 
            return NULL; 
        }

        idx = HASH(id); 
        pthread_mutex_lock (&hashlock); 
        fp->f_next = fh[idx]; 
        fh[idx] = fp; //fp->f_next; 

        pthread_mutex_lock (&fp->f_lock); 
        pthread_mutex_unlock (&hashlock); 
        fp->f_id = id; 
        pthread_mutex_unlock (&fp->f_lock); 
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

struct foo* foo_find (int id)
{
    struct foo* fp; 
    int idx; 
    idx = HASH(id); 
    pthread_mutex_lock (&hashlock); 
    for (fp = fh[idx]; fp != NULL; fp = fp->f_next) { 
        if (fp->f_id == id)
        {
            foo_hold (fp); 
            break; 
        }
    }

    if (fp)
        printf ("thread %lu find me, id %d, ref count %d\n", pthread_self (), fp->f_id, fp->f_count); 
    pthread_mutex_unlock (&hashlock); 
    return fp; 
}

void foo_rele (struct foo *fp)
{
    struct foo *tfp; 
    int idx; 

    pthread_mutex_lock (&fp->f_lock); 
    if (fp->f_count == 1) { 
        pthread_mutex_unlock (&fp->f_lock); 
        pthread_mutex_lock (&hashlock); 
        pthread_mutex_lock (&fp->f_lock); 

        if (fp->f_count != 1)
        {
            fp->f_count --; 
            pthread_mutex_unlock (&fp->f_lock); 
            pthread_mutex_unlock (&hashlock); 
            return; 
        }

        idx = HASH(fp->f_id); 
        tfp = fh[idx]; 
        if (tfp == fp) { 
            fh[idx] = fp->f_next; 
        }
        else { 
            while (tfp->f_next != fp)
                tfp = tfp->f_next; 

            tfp->f_next = fp->f_next; 
        }

        printf ("thread %lu destroy me, ref count %d\n", pthread_self (), fp->f_count); 
        pthread_mutex_unlock (&hashlock); 
        pthread_mutex_unlock (&fp->f_lock); 
        pthread_mutex_destroy (&fp->f_lock); 
        free (fp); 
    }
    else {
        fp->f_count --; 
        printf ("thread %lu release me, ref count %d\n", pthread_self (), fp->f_count); 
        pthread_mutex_unlock (&fp->f_lock); 
    }
}

void* thr_fn (void *arg)
{
    printf ("thread %lu running\n", pthread_self ()); 
    int id = (int) arg; 
    struct foo* fp = foo_find (id); 
    sleep (1); 
    foo_rele (fp); 
    printf ("thread %lu exit\n", pthread_self ()); 
    return NULL; 
}

int main ()
{
    struct foo* fp = foo_alloc (100); 
    if (fp == NULL)
        return -1; 

    int ret = 0; 
    pthread_t tid; 
    ret = pthread_create (&tid, NULL, thr_fn, (void *)100); 
    if (ret != 0)
        err_quit ("create thread failed, errno %d\n", ret); 

    ret = pthread_join (tid, NULL); 
    if (ret != 0)
        err_quit ("join thread failed, errno %d\n", ret); 

    printf ("thread %lu joined\n", tid); 
    foo_rele (fp); 
    return 0; 
}

