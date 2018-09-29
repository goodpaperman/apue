#include "../apue.h" 
#include <pthread.h> 
//#include <errno.h> 

#define TEST_CROSS_LOCK
#define TEST_DEADLOCK_BACKOFF

pthread_mutex_t g_mutex1 = PTHREAD_MUTEX_INITIALIZER; 

#ifdef TEST_CROSS_LOCK
pthread_mutex_t g_mutex2 = PTHREAD_MUTEX_INITIALIZER; 
#endif

void* thr_fn1 (void *arg)
{
    int ret = 0; 
    printf ("thread1 running\n"); 
    while (1)
    {
        pthread_mutex_lock (&g_mutex1); 
        printf ("thread1 lock mutex1\n"); 

#ifdef TEST_CROSS_LOCK
#  ifdef TEST_DEADLOCK_BACKOFF
        ret = pthread_mutex_trylock (&g_mutex2); 
        while (ret != 0)
        {
            printf ("thread1 try lock mutex2 failed (errno %d), release mutex1 to avoid deadlock!\n", ret); 
            pthread_mutex_unlock (&g_mutex1); 
            printf ("thread1 unlock mutex1\n"); 
            usleep (1000); 

            pthread_mutex_lock (&g_mutex1); 
            printf ("thread1 lock mutex1\n"); 
            ret = pthread_mutex_trylock (&g_mutex2); 
        }
#  else
        pthread_mutex_lock (&g_mutex2); 
#  endif 
        printf ("thread1 lock mutex2\n"); 

#endif 

#ifdef TEST_LOCK_TWICE
        pthread_mutex_lock (&g_mutex1); 
        printf ("thread1 lock mutex1 again\n"); 
#endif 

        usleep (1000); 
        
#ifdef TEST_CROSS_LOCK
        pthread_mutex_unlock (&g_mutex2); 
        printf ("thread1 unlock mutex2\n"); 
#endif 

        pthread_mutex_unlock (&g_mutex1); 
        printf ("thread1 unlock mutex1\n"); 
    }

    printf ("thread1 exit\n"); 
    return NULL; 
}

void *thr_fn2 (void *arg)
{
    int ret = 0; 
    printf ("thread2 running\n"); 
    while (1)
    {
#ifdef TEST_CROSS_LOCK
        pthread_mutex_lock (&g_mutex2); 
        printf ("thread2 lock mutex2\n"); 
#endif 

#ifdef TEST_DEADLOCK_BACKOFF
        ret = pthread_mutex_trylock (&g_mutex1); 
        while (ret != 0)
        {
            printf ("thread2 try lock mutex1 failed (errno %d), release mutex2 to avoid deadlock!\n", ret); 
            pthread_mutex_unlock (&g_mutex2); 
            printf ("thread2 unlock mutex2\n"); 
            usleep (1000); 

            pthread_mutex_lock (&g_mutex2); 
            printf ("thread2 lock mutex2\n"); 
            ret = pthread_mutex_trylock (&g_mutex1); 
        }
#else
        pthread_mutex_lock (&g_mutex1); 
#endif
        printf ("thread2 lock mutex1\n"); 

        usleep (1000); 

        pthread_mutex_unlock (&g_mutex1); 
        printf ("thread2 unlock mutex1\n"); 

#ifdef TEST_CROSS_LOCK
        pthread_mutex_unlock (&g_mutex2); 
        printf ("thread2 unlock mutex2\n"); 
#endif 
    }
    printf ("thread2 exit\n"); 
    return NULL; 
}

int main ()
{
    int ret = 0; 
    pthread_t tid1, tid2; 
    ret = pthread_create (&tid1, NULL, thr_fn1, NULL); 
    if (ret != 0)
        err_quit ("create thread failed, errno %d\n", ret); 

    ret = pthread_create (&tid2, NULL, thr_fn2, NULL); 
    if (ret != 0)
        err_quit ("create thread failed, errno %d\n", ret); 

    printf ("create all thread\n"); 
    ret = pthread_join (tid1, NULL); 
    if (ret != 0)
        err_quit ("join thread failed, errno %d\n", ret); 

    ret = pthread_join (tid2, NULL); 
    if (ret != 0)
        err_quit ("join thread failed, errno %d\n", ret); 

    printf ("join all thread\n"); 
    return 0; 
}
