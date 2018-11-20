#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

#define USE_REENT 2
#define THR_MAX 3

#define PASSERT(ret) \
    if ((ret) != 0) \
        err_sys ("pthread api error"); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 

extern char **environ; 




#if USE_REENT==0

static char envbuf[ARG_MAX]; 
char* my_getenv (char const* name)
{
    int i, len; 
    len = strlen (name); 
    for (i=0; environ[i] != NULL; i++) { 
        if ((strncmp (name, environ[i], len) == 0)
            && (environ[i][len] == '=')) { 
            strcpy (envbuf, &environ[i][len+1]); 
            return envbuf; 
        }
    }

    return NULL; 
}

#elif USE_REENT==1

pthread_mutex_t env_mutex; 
static pthread_once_t init_done = PTHREAD_ONCE_INIT; 

static void thread_init (void)
{
    pthread_mutexattr_t attr; 
    PASSERT(pthread_mutexattr_init (&attr)); 
    PASSERT(pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE)); 
    PASSERT(pthread_mutex_init (&env_mutex, &attr)); 
    PASSERT(pthread_mutexattr_destroy (&attr)); 
    printf ("thread init called in %lu\n", pthread_self ()); 
}

int my_getenv (char const* name, char *buf, int buflen)
{
    int i, len, olen; 
    PASSERT(pthread_once (&init_done, thread_init)); 
    len = strlen (name); 
    PASSERT(pthread_mutex_lock (&env_mutex)); 
    for (i=0; environ[i] != NULL; i++) {
        if ((strncmp (name, environ[i], len) == 0) &&
            (environ[i][len] == '=')) { 
            olen = strlen (&environ[i][len+1]); 
            if (olen >= buflen) { 
                PASSERT(pthread_mutex_unlock (&env_mutex)); 
                return ENOSPC; 
            }

            strcpy (buf, &environ[i][len+1]); 
            PASSERT(pthread_mutex_unlock (&env_mutex)); 
            return 0; 
        }
    }

    PASSERT(pthread_mutex_unlock (&env_mutex)); 
    return ENOENT; 
}

#else

static pthread_key_t key; 
static pthread_once_t init_done = PTHREAD_ONCE_INIT; 
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER; 

static void thread_init (void)
{
    PASSERT(pthread_key_create (&key, free)); 
    printf ("thread init called in %lu\n", pthread_self ()); 
}

char* my_getenv (char const* name)
{
    int i, len;
    char *envbuf; 
    PASSERT(pthread_once (&init_done, thread_init)); 
    PASSERT(pthread_mutex_lock (&env_mutex)); 
    envbuf = (char *)pthread_getspecific(key); 
    if (envbuf == NULL) { 
        envbuf = malloc(ARG_MAX); 
        if (envbuf == NULL) {
            PASSERT(pthread_mutex_unlock (&env_mutex));
            return NULL; 
        }
        PASSERT(pthread_setspecific(key, envbuf)); 
    }

    len = strlen(name); 
    for (i=0; environ[i] != NULL; i++) {
        if ((strncmp (name, environ[i], len) == 0) &&
            (environ[i][len] == '=')) { 
            strcpy (envbuf, &environ[i][len+1]); 
            PASSERT(pthread_mutex_unlock (&env_mutex)); 
            return envbuf; 
        }
    }

    PASSERT(pthread_mutex_unlock (&env_mutex)); 
    return NULL; 
}

#endif

void* thr_fun (void *arg)
{
    int i; 
    for (i=0; i<3; ++ i)
    {
        int n = rand () % 20; 
        char *ptr = strchr (environ[n], '='); 
        char key[128] = { 0 }; 
        if (ptr)
            strncpy (key, environ[n], ptr - environ[n]); 
        else
            strcpy (key, environ[n]); 

        printf ("[%lu] test key %s\n", pthread_self (), key); 


        int ret = 0; 
#if USE_REENT==0 || USE_REENT==2
        char const* envstr = my_getenv (key); 
        if (envstr)
#else
        char buf[128] = { 0 }; 
        ret = my_getenv (key, buf, sizeof(buf)); 
        char const* envstr = buf; 
        if (ret == 0)
#endif
            printf ("[%lu] %s: %s\n", pthread_self (), key, envstr); 
        else 
            printf ("[%lu] %s not found, ret %d\n", pthread_self (), key, ret); 
    }

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

    return 0; 
}

