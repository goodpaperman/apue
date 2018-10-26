#include "../apue.h" 
#include <pthread.h> 
#include <limits.h>

// indefinite recursive loop
void* thr_fun (void *arg)
{
    static int n = 0; 
    //int local = 0; 
    //float var = .0f; 
    //double dbl = 1.1;
    //long long ll=1LL; 
    //char buf[48] = { 0 }; 
    printf ("this is turn %d\n", n++); 
    return thr_fun (arg); 
}

//#define USE_MIN_STACK

int main ()
{
    int err = 0; 
    pthread_t tid; 
    pthread_attr_t attr; 

    err = pthread_attr_init (&attr); 
    if (err != 0)
        err_sys ("pthread_attr_init failed\n"); 

#ifdef USE_MIN_STACK
    err = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN); 
#elif USE_MAX_STACK
    err = pthread_attr_setstacksize(&attr, 16384000); 
#else
    err = 0; 
#endif
    if (err != 0)
        err_sys ("pthread_attr_setstacksize failed\n"); 

    err = pthread_create (&tid, &attr, thr_fun, NULL); 
    if (err != 0)
        err_sys ("pthread_create failed\n"); 

    pthread_attr_destroy (&attr); 

    printf ("prepare to join %lu\n", tid); 
    err = pthread_join (tid, NULL); 
    printf ("main thread exit\n"); 
    return 0; 
}
