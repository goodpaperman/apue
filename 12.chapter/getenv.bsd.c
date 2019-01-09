#include <stdio.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <errno.h> 
#if 1
#include <linux/limits.h> 
#else
// on freebsd
#include <sys/syslimits.h> 
#endif
#include <string.h> 

#define THR_MAX 3

#define PASSERT(ret) \
    if ((ret) != 0) \
        printf ("pthread api error:%d\n", errno); \
    else \
        printf ("[%lu] %s\n", pthread_self (), #ret); 

extern char **environ; 

static pthread_key_t key; 
static pthread_once_t init_done = PTHREAD_ONCE_INIT; 
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER; 

static void thread_init (void)
{
  PASSERT(pthread_key_create(&key, free)); 
  printf ("thread init called in %lu\n", pthread_self ()); 
}

struct KEY
{
  pthread_t key; 
  char* buf; 
}; 

#if 1
#define MY_GETENV getenv
#else 
#define MY_GETENV my_getenv
#endif 

char* MY_GETENV (char const* name/*, int dummy*/)
{
    int i, len;
    char *envbuf; 
    PASSERT(pthread_once (&init_done, thread_init)); 
    PASSERT(pthread_mutex_lock (&env_mutex)); 
    envbuf = (char *)pthread_getspecific(key); 
    if (envbuf == NULL) { 
 
#if 1
        envbuf = malloc (ARG_MAX); 
#else   
        static int o = 0; 
        static char buf[THR_MAX][128] = { 0 }; 
        static struct KEY arr[THR_MAX] = { 0 }; 
        for (i=0; i<THR_MAX; i ++)
        {
          if (arr[i].key == pthread_self ())
            break; 
          if (arr[i].key == 0)
            break; 
        }

        if (arr[i].key == 0)
        {
          arr[i].key = pthread_self (); 
          printf ("allocate %p(%d) to %lu\n", buf[o], o, pthread_self ()); 
          arr[i].buf = buf[o++]; 
        }

        envbuf = arr[i].buf; 
        printf ("got %d slot as memory\n", i); 
#endif
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

#if 1
int unsetenv (char const* name)
{
    return 0; 
}
#endif

void* thr_fun (void *arg)
{
  int i, m; 
  for (i=0; environ[i] != NULL; ++ i)
    ; 

  m = i; 
  printf ("total environment %d\n", m);
  for (i=0; i<3; ++ i)
  {
    int n = rand () % m; 
    printf ("got random %d\n", n);
    char *ptr = strchr (environ[n], '='); 
    char key[128] = { 0 }; 
    if (ptr)
      strncpy (key, environ[n], ptr - environ[n]); 
    else 
      strcpy (key, environ[n]); 

    printf ("[%lu] test key %s\n", pthread_self (), key); 
    int ret = 0; 
    char const* envstr = MY_GETENV(key/*, 0*/); 
    if (envstr)
      printf ("[%lu] %s:%s\n", pthread_self (), key, envstr); 
    else 
      printf ("[%lu] %s not found, ret %d\n", pthread_self (), key, ret); 
  }
  return 0; 
}

int main (int argc, char *argv[])
{
  int i; 
  srand (time(NULL)); 
  pthread_t tid[THR_MAX] = { 0 }; 
  for (i=0; i<THR_MAX; ++ i)
    PASSERT(pthread_create(&tid[i], NULL, thr_fun, NULL)); 
  
  for (i=0; i<THR_MAX; ++ i)
    PASSERT(pthread_join (tid[i], NULL)); 

  return 0; 
}
