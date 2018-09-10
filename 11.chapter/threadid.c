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
    while (!que->empty ())
    {
        que->wait_and_pop (data); 
        if (pthread_equal (data.key, pthread_self ()) == 0)
        {
            printf ("[%lu] eat item %s", pthread_self (), data.value.c_str ()); 
        }
        else 
        {
            que->push(data); 
            printf ("[%lu] not my cake, push it back\n", pthread_self ()); 
            usleep (200000); 
        }
    }

    printf ("[%lu] thread exit\n", pthread_self ()); 
    return (void *)1; 

}

int main ()
{
    int i = 0; 
    concurrent_queue<struct thread_data> que; 
    pthread_t tid[MAX_THREADS] = { 0 }; 
    for (i=0; i<3; i++)
    {
        tid[i] = pthread_create(&tid[i], NULL, thread_proc, (void *)&que); 
    }

    for (i=0; i<1000; ++ i)
    {
        // put 1000 tasks
        struct thread_data td; 
        td.key = tid[i%3]; 
        td.value = std::to_string (i); 
        que.push (td); 
    }

    void* status = NULL; 
    printf ("prepare to join\n"); 
    for (i=0; i<3; ++ i)
    {
        pthread_join (tid[i], &status); 
        printf ("wait thread [%lu], exit code [%p]\n", tid[i], status); 
    }

    printf ("main exit\n"); 
    return 0; 
}
