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
            printf ("[%lu %lu] running %d\n", getpid (), pthread_self (), i); 
            sleep (1); 
        }
    }

    return NULL; 
}

int main (int argc, char *argv[])
{
    int i; 
    pthread_t tid[THR_MAX]; 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, (void*)i)); 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    return 0; 
}

