#include "../apue.h" 

#define MAX_THREADS 3

void thread_proc (void *arg)
{
    struct Queue *que = (Queue*)arg; 
    while (!que.empty ())
    {
        if (pthread_equal (que.front().tid (), pthread_self ()) == 0)
        {
            Item it = que.front (); 
            que.pop (); 
            printf ("do work for %d", it.data ()); 
        }
    }

    printf ("thread %d exit\n", pthread_self ()); 

}

int main ()
{
    int i = 0; 
    pthread_t tid[MAX_THREADS] = { 0 }; 
    for (i=0; i<3; i++)
    {
        tid[i] = pthread_create(...); 
    }

    for (i=0; i<1000; ++ i)
    {
        // put 1000 tasks
        queue.push (tid[i%3], i); 
    }

    printf ("prepare to join\n"); 
    for (i=0; i<3; ++ i)
        pthread_join (tid[i]); 

    printf ("main exit\n"); 
    return 0; 
}
