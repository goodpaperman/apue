#include "../apue.h" 
#include <time.h> 

void sigalrm (int signo)
{
    printf ("SIGALRM received\n"); 
}

void do_sleep (int sec)
{
    time_t start = time (NULL); 
    int ret = alrm_sleep (5); 
    time_t end = time (NULL); 
    printf ("alarm return %d, sleep %u, start %lu, end %lu\n", ret, (int)(end - start), start, end); 
}

int main (int argc, char *argv[])
{
    int ret = 0; 
#if 1
    signal (SIGALRM, sigalrm); 
#endif

#if 1
    ret = alarm(10); 
#else 
    ret = alarm(3); 
#endif

    printf ("alarm(n) return %d\n", ret); 
    do_sleep (5); 

#if 0
    ret = alarm (10);
    printf ("alarm(n) return %d\n", ret); 
#endif 

    pause (); 
    return 0; 
}
