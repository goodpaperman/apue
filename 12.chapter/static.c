#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

//#define USE_ONCE
#define THR_MAX 30

#define PASSERT(ret) \
    if ((ret) != 0) {\
        printf ("pthread api error"); \
        exit (1); \
    }\
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 


int my_rand () 
{
    printf ("[%lu] go into my_rand\n", pthread_self ()); 
    sleep (1); 
    return rand () % THR_MAX; 
}

#ifdef USE_ONCE
static int g_val = 0; 
static pthread_once_t init_done = PTHREAD_ONCE_INIT; 

void my_rand_v (void)
{
    g_val = my_rand (); 
    printf ("[%lu] global rand value %d\n", pthread_self (), g_val); 
}
#endif


void* thr_fun (void *arg)
{
#ifdef USE_ONCE
    pthread_once (&init_done, my_rand_v); 
    int val = g_val; 
#else
    static int val = my_rand (); 
#endif 
    printf ("[%lu] rand value %d\n", pthread_self (), val); 
    return 0; 
}


int main (int argc, char *argv[])
{
    int i;
    srand (time (NULL)); 
    pthread_t tid[THR_MAX] = { 0 }; 
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, NULL)); 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    printf ("main exit\n"); 
    return 0; 
}

