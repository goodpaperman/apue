#include "../apue.h" 
#include <pthread.h> 
#include <error.h> 

void* thr_fun (void *arg)
{
    printf ("this is thread %lu (0x%x)\n", pthread_self (), pthread_self ()); 
    sleep (1); 

#if 0
    pthread_exit ((void *)pthread_self ()); 
#endif 

    sleep (1); 
    printf ("exit from child thread\n"); 
    return (void*) 24; 
}

int main ()
{
    pthread_t tid; 
    int err = pthread_create (&tid, NULL, thr_fun, NULL); 
    if (err != 0)
        err_quit ("can't create thread: %s\n", strerror(err)); 

    sleep (1); 

#if 0
    err = pthread_cancel (tid); 
    if (err != 0)
        err_quit ("can't cancel thread: %s\n", strerror (err)); 
    else 
        printf ("cancel child\n"); 
#endif 

    void *status = 0; 
    err = pthread_join (tid, &status); 
    if (err != 0)
        err_quit ("can't join thread: %s\n", strerror (err)); 
    else 
        printf ("wait child, status = 0x%x\n", status); 

    printf ("main thread exit..\n"); 
    return 42; 
}
