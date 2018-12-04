#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

#define THR_MAX 2

#define PASSERT(ret) \
{\
    int __ret=(ret);\
    if (__ret != 0) \
        printf ("pthread api error %s: %d\n", #ret, __ret); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); \
}

pthread_key_t key; 

void my_free (void *arg)
{
    printf ("call destructor for [%lu]\n", pthread_self ()); 
    free (arg); 
}

void* thr_fun (void *arg)
{
    int n, ostate, otype; 
#if 1
    int nstate = PTHREAD_CANCEL_ENABLE;
#else 
    int nstate = PTHREAD_CANCEL_DISABLE; 
#endif

#if 1
    int ntype = PTHREAD_CANCEL_DEFERRED; 
#else
    int ntype = PTHREAD_CANCEL_ASYNCHRONOUS; 
#endif

    char *value = (char *)malloc (10); 
    strcpy (value, "hello\n"); 
    PASSERT(pthread_setspecific (key, value)); 

    PASSERT(pthread_setcancelstate (nstate, &ostate)); 
    printf ("[%lu] start, old cancel state %u\n", pthread_self (), ostate); 

    PASSERT(pthread_setcanceltype (ntype, &otype)); 
    printf ("old cancel type %u\n", otype); 

#if 0
    for (n=0; n<3; ++ n)
    {
        usleep (100000);
        printf ("[%lu] loop %d\n", pthread_self (), n); 
    }
#else
    for (n=0; n<3; ++n)
    {
        int i=0, j=0; 
        double m; 
        for (i=0; i<1000000; ++ i
#if 0
            )
#else
                , pthread_testcancel())
#endif
        {
            for (j=0; j<1000; ++j)
            {
                m = 1.0*(i*i+j*j)/(i*j+i+j+1); 
            }

            // note: printf calls write, and is a cancel point too.
            //printf ("[%lu] loop %d, %d\n", pthread_self (), n, i); 
        }

        //printf ("[%lu] loop %d\n", pthread_self (), n); 
    }
#endif

    //printf ("[%lu] exit with n=%d\n", pthread_self (), n);
    return 0; 
}

int main (int argc, char *argv[])
{
    int i;
    srand (time (NULL)); 
    PASSERT(pthread_key_create (&key, my_free)); 
    pthread_t tid[THR_MAX] = { 0 }; 
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, NULL)); 

    printf ("create all threads\n"); 
    usleep (200000); 
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_cancel (tid[i])); 

    usleep (10000); 
    printf ("cancel all threads\n"); 
    void* status; 
    for (i=0; i<THR_MAX; ++ i)
    {
        PASSERT(pthread_join (tid[i], &status)); 
        printf ("join thread %lu with status 0x%x\n", tid[i], status); 
    }

    printf ("join all threads\n"); 
    PASSERT(pthread_key_delete (key)); 
    return 0; 
}

