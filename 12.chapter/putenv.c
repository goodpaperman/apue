#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

#define THR_MAX 3
#define LOOP 100

#define PASSERT(ret) \
    if ((ret) != 0) \
        err_sys ("pthread api error"); \
    else \
        printf ("[%lu] %s\n", pthread_self(),  #ret); 

extern char **environ; 

static pthread_key_t key; 
static pthread_once_t init_done = PTHREAD_ONCE_INIT; 
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER; 

void my_free (void *arg)
{
    int i; 
    printf ("[%lu] freeing 0x%x\n", pthread_self (), arg); 
    char **envbuf = (char **) arg; 
    for (i=0; envbuf[i] != NULL; ++ i)
    {
        printf ("free %d: %s\n", i, envbuf[i]); 
        free (envbuf[i]); 
    }

    free ((char *)envbuf); 
}

static void thread_init (void)
{
    PASSERT(pthread_key_create (&key, my_free)); 
    printf ("thread init called in %lu\n", pthread_self ()); 
}

int my_putenv (char const* name, char const* value)
{
    int i, len;
    char **envbuf; 
    PASSERT(pthread_once (&init_done, thread_init)); 
    PASSERT(pthread_mutex_lock (&env_mutex)); 
    envbuf = (char *)pthread_getspecific(key); 
    if (envbuf == NULL) { 
        envbuf = (char **)malloc(ARG_MAX*sizeof(char *)); 
        if (envbuf == NULL) {
            printf ("fatal error, no mem!\n"); 
            PASSERT(pthread_mutex_unlock (&env_mutex));
            return -1; 
        }

        for (i = 0; environ[i] != NULL; ++ i)
        {
            envbuf[i] = malloc (strlen (environ[i]) + 1); 
            strcpy (envbuf[i], environ[i]); 
        }

        envbuf[i] = NULL; 
        printf ("[%lu] setup %d items in new environment string\n", pthread_self (), i); 
        PASSERT(pthread_setspecific(key, (void *)envbuf)); 
    }

    len = strlen(name); 
    for (i=0; envbuf[i] != NULL; i++) {
        if ((strncmp (name, envbuf[i], len) == 0) &&
            (envbuf[i][len] == '=')) { 
            // find existing item
            free (envbuf[i]); 
            printf ("replace existing one\n"); 
            break; 
        }
    }

    // not find, insert one
    if (envbuf[i] == NULL)
        printf ("insert one\n"); 

    envbuf[i] = malloc (strlen(name) + strlen(value) + 2); 
    sprintf (envbuf[i], "%s=%s", name, value); 
    printf ("set %s: %s\n", name, value); 
    PASSERT(pthread_mutex_unlock (&env_mutex)); 
    return 0; 
}


void* thr_fun (void *arg)
{
    int i; 
    for (i=0; i<LOOP; ++ i)
    {
        int n = rand () % 20; 
        char key[128] = { 0 }; 
#if 1
        sprintf (key, "demo_%03d", n); 
#else 
        char *ptr = strchr (environ[n], '='); 
        if (ptr)
            strncpy (key, environ[n], ptr - environ[n]); 
        else
            strcpy (key, environ[n]); 
#endif

        char value[128] = { 0 }; 
        sprintf (value, "test_%02d", n); 
        printf ("[%lu] test key %s, value %s\n", pthread_self (), key, value); 


        int ret = 0; 
        ret = my_putenv (key, value); 
        if (ret == 0)
            printf ("[%lu] %s: %s\n", pthread_self (), key, value); 
        else 
            printf ("[%lu] %s insert failed, ret %d\n", pthread_self (), key, ret); 
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

