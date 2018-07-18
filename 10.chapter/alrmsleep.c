#include "../apue.h" 

void sigalrm (int signo)
{
    printf ("SIGALRM received\n"); 
}

void sigint (int signo)
{
    int i, j; 
    volatile int k; 
    printf ("sigint starting\n"); 
    for (i=0; i<300000; i++)
        for (j=0; j<4000; j++)
            k += i * j; 

    printf ("sigint finished\n"); 
}

int main (int argc, char *argv[])
{
    int ret = 0; 
#if 1
    signal (SIGINT, sigint); 
    signal (SIGALRM, sigalrm); 
#endif

#if 0
    ret = alarm(10); 
#else 
    ret = alarm(3); 
#endif

    printf ("alarm(n) return %d\n", ret); 
    alrm_sleep (5); 

#if 0
    ret = alarm (10);
    printf ("alarm(n) return %d\n", ret); 
#endif 

    pause (); 
    return 0; 
}
