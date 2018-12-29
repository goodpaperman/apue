#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

#define THR_MAX 2
#define LOOP 3

#define PASSERT(ret) \
{\
    int __ret=(ret);\
    if (__ret != 0) {\
        printf ("pthread api error %s: %d\n", #ret, __ret); \
        exit (__ret); \
    }\
    else \
        printf ("[%lu %lu] %s\n", getpid (), pthread_self(),  #ret); \
}

pthread_mutex_t foo_lock; 
pthread_mutex_t bar_lock; 

void call_bar (int i)
{
    PASSERT(pthread_mutex_lock (&bar_lock)); 
    printf ("[%lu %lu] call bar %d\n", getpid (), pthread_self (), i); 
    sleep (i); 
    PASSERT(pthread_mutex_unlock (&bar_lock)); 
}

void call_foo (int i)
{
    PASSERT(pthread_mutex_lock (&foo_lock)); 
    printf ("[%lu %lu] call foo %d\n", getpid (), pthread_self (), i); 
    call_bar (i); 
    PASSERT(pthread_mutex_unlock (&foo_lock)); 
}

void* thr_fun (void *arg)
{
    int n = (int)arg, i = 0; 
    if (n == 0)
    {
        sleep (1); 
        printf ("[%lu %lu] before fork\n", getpid (), pthread_self ()); 
        pid_t pid = fork (); 
        if (pid < 0)
        {
            exit (-1); 
        }
        else if (pid == 0)
        {
            // child
            printf ("[%lu %lu] after fork\n", getpid (), pthread_self ()); 
        }
        else 
        {
            // parent
            printf ("[%lu %lu] after fork\n", getpid (), pthread_self ()); 
        }

        //for (i=0; i<LOOP; ++ i)
        //{
        //    printf ("[%lu %lu] running %d\n", getpid (), pthread_self (), i); 
        //    sleep (1); 
        //}
    }
    //else 
    {
        for (i=0; i<LOOP; ++ i)
        {
            call_foo (i); 
        }
    }

    return NULL; 
}

void prepare (void)
{
    printf ("[%lu %lu] preparing locks...\n", getpid (), pthread_self ()); 
    PASSERT(pthread_mutex_lock (&foo_lock)); 
    PASSERT(pthread_mutex_lock (&bar_lock)); 
}

void parent (void)
{
    printf ("[%lu %lu] parent unlocking...\n", getpid (), pthread_self ()); 
    PASSERT(pthread_mutex_unlock (&bar_lock)); 
    PASSERT(pthread_mutex_unlock (&foo_lock)); 
}

void child (void)
{
    printf ("[%lu %lu] child unlocking...\n", getpid (), pthread_self ()); 
    PASSERT(pthread_mutex_unlock (&bar_lock)); 
    PASSERT(pthread_mutex_unlock (&foo_lock)); 
}

int main (int argc, char *argv[])
{
    int i; 
    pthread_t tid[THR_MAX]; 
    PASSERT(pthread_atfork (prepare, parent, child)); 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, (void*)i)); 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    return 0; 
}

