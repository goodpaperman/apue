#include "../apue.h" 
#include <pthread.h> 

struct foo { 
    int a, b, c, d; 
}; 

void printfoo (const char* s, struct foo const* fp)
{
    printf ("%s", s); 
    printf (" structure at 0x%x\n", (unsigned) fp); 
    printf (" foo.a = %d\n", fp->a); 
    printf (" foo.b = %d\n", fp->b); 
    printf (" foo.c = %d\n", fp->c); 
    printf (" foo.d = %d\n", fp->d); 
}

void* thr_fn1 (void *arg)
{
#ifdef USE_STACK 
    struct foo fp = { 1, 2, 3, 4 }; 
    struct foo* f = &fp; 
#else 
    struct foo *f = (struct foo*) malloc (sizeof (struct foo)); 
    f->a = 1; 
    f->b = 2; 
    f->c = 3; 
    f->d = 4; 
#endif 
    printfoo ("thread 1: \n", f); 
    pthread_exit ((void *)f); 
}

void* thr_fn2 (void *arg)
{
    printf ("thread 2: ID is %lu\n", pthread_self ()); 
    pthread_exit ((void *)0); 
}

int main (void)
{
    int err; 
    pthread_t tid1, tid2; 
    struct foo *fp; 
    err = pthread_create (&tid1, NULL, thr_fn1, NULL); 
    if (err != 0)
        err_quit ("can't create thread 1: %s\n", strerror (err)); 

    err = pthread_join (tid1, (void *)&fp); 
    if (err != 0)
        err_quit ("can't join with thread 1: %s\n", strerror (err)); 

#if 0
    sleep (1); 

    printf ("parent starting second thread\n"); 
    err = pthread_create (&tid2, NULL, thr_fn2, NULL); 
    if (err != 0)
        err_quit ("can't create thread 2: %s\n", strerror (err)); 

    sleep (1); 
#endif 

    printfoo ("parent: \n", fp); 
#ifndef USE_STACK
    free (fp); 
#endif

    exit (0); 
}
