#include "../apue.h" 
#include <pthread.h> 
#include <limits.h>

// indefinite recursive loop
void* thr_fun (void *arg)
{
    static int n = 0; 
    printf ("this is turn %d\n", n++); 
    sleep (1); 
    return NULL; //thr_fun (arg); 
}

int main ()
{
    int err = 0; 
    pthread_t tid; 
    err = pthread_create (&tid, NULL, thr_fun, NULL); 
    if (err != 0)
        err_sys ("pthread_create failed\n"); 

    int con = pthread_getconcurrency (); 
    printf ("concurrent = %d\n", con); 
    err = pthread_setconcurrency (10); 
    if (err != 0)
        err_sys ("pthread_setconcurrency failed\n"); 

    con = pthread_getconcurrency (); 
    printf ("new concurrent = %d\n", con); 

    printf ("prepare to join %lu\n", tid); 
    err = pthread_join (tid, NULL); 
    printf ("main thread exit\n"); 
    return 0; 
}
