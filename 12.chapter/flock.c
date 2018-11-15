#include "../apue.h" 
#include <pthread.h> 

//#define USE_LOCK
#define MAX_THREADS 3
#define PASSERT(ret) \
    if ((ret) != 0) \
        err_sys ("pthread api error"); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER; 

void* thr_fun (void *arg)
{
    int n = 0, m = 0; 
    char buf[1024] = { 0 }; 
    FILE* fp = (FILE*) arg; 

    flockfile (fp); 
    printf ("%lu occupy file\n", pthread_self ()); 
    for (n=0; n<100; ++ n)
    {
        sprintf(buf, "%lu\t%d\n", pthread_self (), n); 
#ifdef USE_LOCK
        m = fputs (buf, fp); 
#else
        m = fputs_unlocked (buf, fp);
#endif
        printf ("%lu puts %d\n", pthread_self (), m); 
    }

    funlockfile (fp); 
    printf ("%lu work done!\n", pthread_self ()); 
    return NULL; 
}

int main (void)
{
    int i = 0;
    void *code = NULL; 
    unlink ("a.log"); 
    FILE* fp = fopen ("a.log", "a"); 
    if (fp == NULL)
        err_sys ("fopen failed\n"); 

    pthread_t tid[MAX_THREADS] = { 0 }; 
    for (i = 0; i<MAX_THREADS; ++ i)
    {
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, (void*)fp)); 
    }
    
    printf ("create all threads done!\n"); 
    
    for (i=0; i<MAX_THREADS; ++ i)
    {
        PASSERT(pthread_join (tid[i], &code)); 
    }

    printf ("join all threads done!\n"); 
    fclose (fp); 
    return 0; 
}

