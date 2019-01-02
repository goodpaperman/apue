#include "../apue.h" 
#include <linux/limits.h> 
#include <errno.h>
#include <pthread.h> 

#define THR_MAX 2
#define LOOP 50

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

int fd = 0; 

void* thr_fun (void *arg)
{
    char c = '0'; 
    int n = (int)arg, i; 
    for (i=0; i<LOOP; ++ i)
    {
#if 0
        lseek (fd, n, SEEK_SET); 
        usleep (10000); 
        read (fd, &c, 1); 
#else 
        pread (fd, &c, 1, n); 
        usleep (10000); 
#endif
        printf ("[%lu] read '%c' at %d\n", pthread_self (), c, n); 
    }

    return NULL; 
}

int main (int argc, char *argv[])
{
    int i; 
    fd = open ("./pread.c", O_RDWR); 
    pthread_t tid[THR_MAX]; 
    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_create (&tid[i], NULL, thr_fun, (void *)i)); 

    for (i=0; i<THR_MAX; ++ i)
        PASSERT(pthread_join (tid[i], NULL)); 

    close (fd); 
    return 0; 
}

