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
    int err = 0; 
    pthread_t tid; 
    pthread_attr_t *pattr = NULL; 

#ifdef STARTUP_DETACH
    pthread_attr_t att; 
    pattr = &att; 
    err = pthread_attr_init (&att); 
    if (err != 0)
        err_quit ("attr init failed\n"); 

    err = pthread_attr_setdetachstate (&att, 1); 
    if (err != 0)
        err_quit ("set detach state failed\n"); 
#endif

    err = pthread_create (&tid, pattr, thr_fun, NULL); 
    if (err != 0)
        err_quit ("can't create thread: %s\n", strerror(err)); 

#ifdef STARTUP_DETACH
    err = pthread_attr_destroy (&att); 
    if (err != 0)
        err_quit ("attr destroy failed\n"); 
#else 
    err = pthread_detach (tid); 
    if (err != 0)
        err_quit ("can't detach thread: %s\n", strerror (err)); 

    printf ("thread detached OK\n"); 
#endif 

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
        printf ("can't join thread: %s\n", strerror (err)); 
    else 
        printf ("wait child, status = 0x%x\n", status); 

    printf ("main thread exit..\n"); 
    sleep (1); 
    return 42; 
}

