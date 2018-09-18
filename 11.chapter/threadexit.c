#include "../apue.h" 
#include <pthread.h> 
#include <error.h> 

void sig_main_int (int signo)
{
    printf ("main caught %d\n", signo); 
#if 0
    exit (24); 
#endif 
}

void sig_thr_int (int signo)
{
    printf ("thread caught %d\n", signo); 
#if 0
    exit (34); 
#endif 
}

void* thr_fun (void *arg)
{
    printf ("this is thread %lu\n", pthread_self ()); 
#if 1
    signal (SIGINT, sig_thr_int); 
#endif 

    sleep (1); 

#if 0
    exit (pthread_self ()); 
#endif 
    sleep (1); 
    printf ("exit from child thread\n"); 
    return NULL; 
}

int main ()
{
    pthread_t tid; 
    signal (SIGINT, sig_main_int); 
    int err = pthread_create (&tid, NULL, thr_fun, NULL); 
    if (err != 0)
        err_quit ("can't create thread: %s\n", strerror(err)); 

    sleep (1); 
    err = pthread_kill (tid, SIGINT); 
    printf ("kill thread by SIGINT return %d\n", err); 

    err = pthread_join (tid, NULL); 
    if (err != 0)
        err_quit ("can't join thread: %s\n", strerror (err)); 

    printf ("main thread exit..\n"); 
    return 42; 
}
