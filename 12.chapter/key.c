#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

#define THR_MAX 3
#define KEY_MAX 3

#define PASSERT(ret) \
{\
    int __ret=(ret);\
    if (__ret != 0) \
        printf ("pthread api error %s: %d\n", #ret, __ret); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); \
}


extern char **environ; 
static pthread_key_t key[KEY_MAX]; 
static pthread_once_t init_done = PTHREAD_ONCE_INIT; 
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER; 

void my_free (void *arg)
{
    printf ("[%lu] freeing 0x%x: %s\n", pthread_self (), arg, (char *)arg); 
    int n = atoi ((char *)arg); 
    free (arg); 
}

static void thread_init (void)
{
    int i; 
    for (i=0; i<KEY_MAX; ++ i)
    {
        PASSERT(pthread_key_create (&key[i], my_free)); 
    }

    printf ("thread init called in %lu\n", pthread_self ()); 
}

char* my_getenv (int i)
{
    int len;
    char *envbuf; 
    PASSERT(pthread_once (&init_done, thread_init)); 
    PASSERT(pthread_mutex_lock (&env_mutex)); 

    envbuf = (char *)pthread_getspecific(key[i]); 
    if (envbuf == NULL) { 
        envbuf = (char *)malloc(ARG_MAX); 
        if (envbuf == NULL) {
            PASSERT(pthread_mutex_unlock (&env_mutex));
            return NULL; 
        }
        
        printf ("%lu create 0x%x\n", pthread_self (), envbuf); 
        PASSERT(pthread_setspecific(key[i], envbuf)); 
        sprintf (envbuf, "%d", rand () % 100); 
    }

    PASSERT(pthread_mutex_unlock (&env_mutex)); 
    return envbuf; 
}

void* thr_fun (void *arg)
{
    int n, i; 
    for (n=0; n<3; ++ n)
    {
        for (i=0; i<KEY_MAX; ++ i)
        {
            int ret = 0; 
            char const* envstr = my_getenv (i); 
            if (envstr)
                printf ("[%lu] %d %s\n", pthread_self (), i, envstr); 
            else 
                printf ("[%lu] %d not found, ret %d\n", pthread_self (), i, ret); 
        }

        usleep (100000);
    }

    return 0; 
}

void delete_key()
{
    int i; 
    for (i=0; i<KEY_MAX; ++ i)
    {
        PASSERT(pthread_key_delete (key[i])); 
    } 
}

int main (int argc, char *argv[])
{
    int i;
    srand (time (NULL)); 
    pthread_t tid[THR_MAX] = { 0 }; 
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, NULL)); 

#if 0
    usleep (299999);  // no auto delete
    //usleep (311111);  // has auto delete
    delete_key (); 
#endif


    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    delete_key (); 
    return 0; 
}

