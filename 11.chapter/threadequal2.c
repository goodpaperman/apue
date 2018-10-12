#include <stdlib.h> 
#include "../apue.h"
#include <pthread.h> 

struct job { 
    struct job *j_next; 
    struct job *j_prev; 
    pthread_t j_id; 
    char value[128]; 
}; 

struct queue { 
    struct job *q_head; 
    struct job *q_tail; 
    pthread_rwlock_t q_lock; 
}; 

int queue_init (struct queue *qp)
{
    int err = 0; 
    qp->q_head = qp->q_tail = NULL; 
    err = pthread_rwlock_init (&qp->q_lock, NULL); 
    if (err != 0)
        return err; 

    return 0; 
}

void dump_queue (struct queue *qp)
{
#ifdef DEBUG
    if (qp->q_head)
        printf ("head 0x%x, value %s, head prev 0x%x, head next 0x%x\n", qp->q_head, qp->q_head->value, qp->q_head->j_prev, qp->q_head->j_next); 
    else 
        printf ("head NULL\n"); 

    if (qp->q_tail)
        printf ("tail 0x%x, value %s, tail prev 0x%x, tail next 0x%x\n", qp->q_tail, qp->q_tail ? qp->q_tail->value : "nil", qp->q_tail->j_prev, qp->q_tail->j_next); 
    else 
        printf ("tail NULL\n"); 
#endif
}

void queue_destroy (struct queue *qp)
{
    struct job *j1, *j2; 
    int n = 0; 
    for (j1 = qp->q_head; j1 != NULL; j1 = j2)
    {
        j2 = j1->j_next; 
        free (j1); 
        n ++; 
    }

    printf ("queue destroy, free %d nodes\n", n); 
    qp->q_head = qp->q_tail = NULL; 
    pthread_rwlock_destroy (&qp->q_lock); 
}

void job_insert (struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock (&qp->q_lock); 
    jp->j_next = qp->q_head; 
    jp->j_prev = NULL; 
    if (qp->q_head != NULL)
        qp->q_head->j_prev = jp; 
    else 
        qp->q_tail = jp; 

    qp->q_head = jp; 
    pthread_rwlock_unlock (&qp->q_lock); 
}

void job_append (struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock (&qp->q_lock); 
    jp->j_next = NULL; 
    jp->j_prev = qp->q_tail; 
    if (qp->q_tail != NULL)
        qp->q_tail->j_next = jp; 
    else 
        qp->q_head = jp; 

    qp->q_tail = jp; 
    pthread_rwlock_unlock (&qp->q_lock); 
}

void job_remove (struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock (&qp->q_lock); 
    if (jp == qp->q_head)
    {
        dump_queue (qp); 
        qp->q_head = jp->j_next; 
        if (qp->q_head)
            qp->q_head->j_prev = NULL; 
        //if (qp->q_tail == jp)
        else
            qp->q_tail = NULL; 

        dump_queue (qp); 
    }
    else if (jp == qp->q_tail)
    {
        dump_queue (qp); 
        qp->q_tail = jp->j_prev; 
        if (qp->q_tail)
            qp->q_tail->j_next = NULL; 
        //if (qp->q_head == jp)
        else
            qp->q_head = NULL; 

        dump_queue (qp); 
    }
    else 
    {
        jp->j_prev->j_next = jp->j_next; 
        jp->j_next->j_prev = jp->j_prev; 
    }

    pthread_rwlock_unlock (&qp->q_lock); 
}

struct job* job_find (struct queue *qp, pthread_t id)
{
    struct job *jp = NULL; 
    if (pthread_rwlock_rdlock (&qp->q_lock) != 0)
        return NULL; 

    for (jp=qp->q_head; jp != NULL; jp=jp->j_next)
        if (pthread_equal (jp->j_id, id))
            break; 

    pthread_rwlock_unlock (&qp->q_lock); 
    return jp; 
}


#define MAX_THREADS 3
#define MAX_JOBS 1000
void* thread_proc (void *arg)
{
    struct job *jp = NULL; 
    struct queue *que = (struct queue *)arg; 
    sleep (1); 
    while ((jp = job_find (que, pthread_self ())) != NULL)
    {
        if (pthread_equal (jp->j_id, pthread_self ()))
        {
            // equal !
            printf ("[0x%x] eat item %s\n", pthread_self (), jp->value); 
            job_remove (que, jp);
            free (jp); 
        }
        else 
        {
            printf ("[0x%x] %s is not my cake (0x%x), fatal error!\n", pthread_self (), jp->value, jp->j_id); 
            //usleep (10000); 
        }
    }

    printf ("[0x%x] thread exit\n", pthread_self ()); 
    return (void *)1; 

}

int main ()
{
    int i = 0, ret = 0; 
    struct queue que; 
    queue_init (&que); 

    pthread_t tid[MAX_THREADS] = { 0 }; 
    for (i=0; i<MAX_THREADS; i++)
    {
        ret = pthread_create(&tid[i], NULL, &thread_proc, (void *)&que); 
        printf ("create thread 0x%x return %d\n", tid[i], ret); 
    }

    char buf[128] = { 0 }; 
    for (i=0; i<MAX_JOBS; ++ i)
    {
        // put 1000 tasks
        struct job *jp = (struct job *)malloc (sizeof (struct job)); 
        jp->j_id = tid[i % MAX_THREADS]; 
        sprintf (jp->value, "%d", i); 
        job_append (&que, jp); 
        //printf ("add item for 0x%x\n", jp->j_id); 
    }

    printf ("setup queue with %u nodes\n", i); 
    void* status = NULL; 
    printf ("prepare to join\n"); 

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
    queue_destroy (&que); 
    return 0; 
}
