#include <unistd.h> 
#include <time.h> 
#include <sys/select.h> 
#include <pthread.h> 
#include <signal.h> 

#define THR_MAX 5

#define PASSERT(ret) \
{\
    int __ret=(ret);\
    if (__ret != 0) {\
        printf ("pthread api error %s: %d\n", #ret, __ret); \
        exit (__ret); \
    }\
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); \
}

unsigned my_sleep (unsigned nsec)
{
    int n; 
    unsigned slept; 
    time_t start, end; 
    struct timeval tv; 
    tv.tv_sec = nsec; 
    tv.tv_usec = 0; 
    time (&start); 
    n = select (0, NULL, NULL, NULL, &tv); 
    if (n == 0)
        return 0; 

    time (&end); 
    slept = end - start; 
    if (slept >= nsec)
        return 0; 

    return nsec - slept; 
}

void sig_int (int signo)
{
    printf ("[%lu] caught %d\n", pthread_self (), signo); 
    my_sleep (THR_MAX); 
    printf ("[%lu] wake from sleep in signal handler\n", pthread_self ()); 
}

void* thr_fun (void *arg)
{
    int n = (int)arg; 
    printf ("[%lu] running\n", pthread_self ()); 
    my_sleep (n); 
    printf ("[%lu] sleep %d seconds\n", pthread_self (), n); 
    return NULL; 
}

int main (int argc, char *argv[])
{
    int i; 
    pthread_t tid[THR_MAX]; 
    signal (SIGINT, sig_int); 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, (void*)i)); 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    return 0; 
}

