#include <stdlib.h> 
#include "../apue.h"
#include <pthread.h> 

struct msg {
    struct msg* m_next; 
    pthread_t m_id; 
    char value[128]; 
}; 

volatile int g_exit = 0; 
struct msg *workq; 
pthread_cond_t qready = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER; 

void process_msg (void)
{
    struct msg *mp; 
    while (!g_exit)
    {
        pthread_mutex_lock (&qlock); 
        while (workq == NULL || pthread_equal (workq->m_id, pthread_self ()) == 0)
        {
            if (g_exit)
            {
                break; 
            }

            if (workq)
                printf ("[0x%x] %s is not my cake (0x%x)\n", pthread_self (), workq->value, workq->m_id); 
            
            pthread_cond_wait (&qready, &qlock); 
        }

        if (g_exit)
        {
            pthread_mutex_unlock (&qlock); 
            break; 
        }

        mp = workq; 
        workq = mp->m_next; 
        pthread_mutex_unlock (&qlock); 

        printf ("[0x%x] eat item %s\n", pthread_self (), mp->value); 
        free (mp); 
    }

    printf ("[0x%x] thread exit\n", pthread_self ()); 
}

void enqueue_msg (struct msg *mp)
{
    pthread_mutex_lock (&qlock); 
    mp->m_next = workq; 
    workq = mp; 
    pthread_mutex_unlock (&qlock); 
    //pthread_cond_signal (&qready); 
    // use broadcast to notify all thread up 
    // and give the right thread a chance to take item out
    pthread_cond_broadcast (&qready); 
}

void dump_queue ()
{
#ifdef DEBUG
    struct msg *m = workq;  
    while (m)
    {
        printf ("head 0x%x, value %s, next 0x%x\n", m, m->value, m->m_next); 
        m = m->m_next; 
    }
#endif
}

void queue_destroy ()
{
    int n = 0; 
    struct msg *m, *p; 
    for (m = workq; m != NULL; )
    {
        p = m; 
        m = m->m_next; 
        free (p); 
        n++; 
    }

    printf ("queue destroy, free %d nodes\n", n); 
    workq = NULL; 
}


#define MAX_THREADS 3
#define MAX_JOBS 1000
void* thread_proc (void *arg)
{
    process_msg (); 
    return (void *)1; 
}

int main ()
{
    int i = 0, ret = 0; 
    pthread_t tid[MAX_THREADS] = { 0 }; 
    for (i=0; i<MAX_THREADS; i++)
    {
        ret = pthread_create(&tid[i], NULL, &thread_proc, NULL); 
        printf ("create thread 0x%x return %d\n", tid[i], ret); 
    }

    sleep (1); 
    for (i=0; i<MAX_JOBS; ++ i)
    {
        // put 1000 tasks
        struct msg *m = (struct msg *)malloc (sizeof (struct msg)); 
        m->m_id = tid[i % MAX_THREADS]; 
        sprintf (m->value, "%d", i); 
        enqueue_msg (m); 
        usleep (5000); 
        printf ("add item %s\n", m->value); 
    }

    printf ("setup queue with %u nodes\n", i); 
    g_exit = 1; 
    void* status = NULL; 
    printf ("prepare to join\n"); 
    pthread_cond_broadcast (&qready); 
    printf ("after boradcast\n"); 

#if 1
    for (i=0; i<MAX_THREADS; ++ i)
    {
        ret = pthread_join (tid[i], &status); 
        printf ("wait thread [0x%x] ret %d, exit code [%p]\n", tid[i], ret, status); 
    }
#else
    sleep (10); 
#endif

    printf ("main exit\n"); 
    dump_queue (); 
    queue_destroy (); 
    return 0; 
}

