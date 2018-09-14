#include "../apue.h" 
#include "concurrent_queue.hpp"

#define MAX_THREADS 3
struct thread_data
{
    pthread_t key; 
    std::string value; 
}; 

void* thread_proc (void *arg)
{
    struct thread_data data; 
    concurrent_queue<struct thread_data> *que = (concurrent_queue<struct thread_data> *)arg; 
    sleep (1); 
    while (!que->empty ())
    {
        que->wait_and_pop (data); 
        if (pthread_equal (data.key, pthread_self ()) != 0)
        {
            // equal !
            printf ("[0x%x] eat item %s, left %u\n", pthread_self (), data.value.c_str (), que->size ()); 
        }
        else 
        {
            que->push(data); 
            printf ("[0x%x] %s is not my cake (0x%x), push it back, left %u\n", pthread_self (), data.value.c_str (), data.key, que->size ()); 
            //usleep (10000); 
        }
    }

    printf ("[0x%x] thread exit\n", pthread_self ()); 
    return (void *)1; 

}

int main ()
{
    int i = 0, ret = 0; 
    concurrent_queue<struct thread_data> que; 
    pthread_t tid[MAX_THREADS] = { 0 }; 
    printf ("sizeof (pthread_t) = %u\n", sizeof (pthread_t)); 
    for (i=0; i<MAX_THREADS; i++)
    {
        ret = pthread_create(&tid[i], NULL, &thread_proc, (void *)&que); 
        printf ("create thread 0x%x return %d\n", tid[i], ret); 
    }

    for (i=0; i<1000; ++ i)
    {
        // put 1000 tasks
        struct thread_data td; 
        td.key = tid[i % MAX_THREADS]; 
        td.value = std::to_string (i); 
        que.push (td); 
        //printf ("add item for 0x%x\n", td.key); 
    }

    printf ("setup queue with %u nodes\n", que.size ()); 
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

    printf ("main exit, queue item left %d\n", que.size ()); 
    return 0; 
}
