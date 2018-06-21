#include "../apue.h" 
#include <signal.h>

int g_total = 0; 

void sig_handler (int signum)
{
    //usleep (20); 
#ifdef USE_SIGNAL
    __sighandler_t old = signal (signum, sig_handler); 
    printf ("reinstall sig_handler, previous: 0x%x\n", old); 
#endif 

    g_total ++; 
    printf ("receive signal %d total %d times\n", signum, g_total); 
}

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        printf ("Usage: lostsig 2\n"); 
        return -1; 
    }

    int signo = atoi (argv[1]); 
#ifdef USE_SIGNAL
    signal (signo, sig_handler); 
#else 
    struct sigaction act; 
    act.sa_handler = sig_handler; 
    sigemptyset(&act.sa_mask); 
    sigaddset(&act.sa_mask, signo); 
    act.sa_flags = 0; 
    sigaction (signo, &act, NULL); 
#endif 
    while (1)
    {
        pause (); 
        printf ("wake up from pause\n"); 
    }

    return 0; 
}
