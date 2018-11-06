#include "../apue.h" 
#include <pthread.h> 
#include <sys/time.h> 

int makethread (void* (*fn) (void*), void *arg)
{
    int err; 
    pthread_t tid; 
    pthread_attr_t attr; 
    err = pthread_attr_init (&attr); 
    if (err != 0)
        return err; 

    err = pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); 
    if (err == 0)
        err = pthread_create (&tid, &attr, fn, arg); 

    pthread_attr_destroy (&attr); 
    return err; 
}

struct to_info {
    void (*to_fn) (void *); 
    void *to_arg; 
    struct timespec to_wait; 
};

#define SEC2NSEC 1000000000
#define USEC2NSEC 1000

void* timeout_helper (void *arg)
{
    struct to_info *tip; 
    tip = (struct to_info *) arg; 
    printf ("before sleep %d sec, %d nsec\n", tip->to_wait.tv_sec, tip->to_wait.tv_nsec); 
    nanosleep (&tip->to_wait, NULL); 
    printf ("sleep over, call!\n"); 
    (*tip->to_fn) (tip->to_arg); 
    return 0; 
}

void timeout (const struct timespec *when, void (*func) (void*), void *arg)
{
    struct timespec now; 
    struct timeval tv; 
    struct to_info *tip; 
    int err; 

    gettimeofday (&tv, NULL); 
    now.tv_sec = tv.tv_sec; 
    now.tv_nsec = tv.tv_usec * USEC2NSEC; 
    if ((when->tv_sec > now.tv_sec) || 
        (when->tv_sec == now.tv_sec && when->tv_nsec > now.tv_nsec)) { 
        tip = malloc (sizeof (struct to_info)); 
        if (tip) {
            tip->to_fn = func; 
            tip->to_arg = arg; 
            tip->to_wait.tv_sec = when->tv_sec - now.tv_sec; 
            if (when->tv_nsec >= now.tv_nsec) { 
                tip->to_wait.tv_nsec = when->tv_nsec - now.tv_nsec; 
            } else { 
                tip->to_wait.tv_sec --; 
                tip->to_wait.tv_nsec = SEC2NSEC - now.tv_nsec + when->tv_nsec; 
            }

            err = makethread(timeout_helper, (void *)tip); 
            if (err == 0)
                return; 
        }
    }

    (*func) (arg); 
}

pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP; 

void retry (void *arg)
{
    pthread_mutex_lock (&mutex); 
    printf ("doing retry ...\n"); 
    pthread_mutex_unlock (&mutex); 
}


int main ()
{
    int err = 0; 
    void *arg = 1; 
    srand (time (0)); 
    struct timespec when; 
    struct timeval tv; 
    gettimeofday (&tv, NULL); 
    when.tv_sec = tv.tv_sec;
    when.tv_nsec = tv.tv_usec * USEC2NSEC + rand () % 50000; 


    pthread_t tid; 
    pthread_mutex_lock (&mutex); 
    timeout (&when, retry, arg); 
    //getchar (); 
    pthread_mutex_unlock (&mutex); 
    printf ("main thread exit\n"); 
    getchar (); 
    return 0; 
}
