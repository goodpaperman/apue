#include "../apue.h" 
#include <sys/wait.h> 

#define USE_SIG 2

static void sig_cld (int); 

int main ()
{
    pid_t pid = 0; 
#if USE_SIG == 1
    __sighandler_t ret = signal (SIGCLD, sig_cld);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#elif USE_SIG == 2
    __sighandler_t ret = signal (SIGCLD, SIG_IGN);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#endif
    if ((pid = fork ()) < 0)
        perror ("fork error"); 
    else if (pid == 0) 
    {
        sleep (2); 
        printf ("child exit\n"); 
        _exit (0); 
    }

#if USE_SIG == 2
    int status = 0; 
    if ((pid = wait (&status)) < 0)
        perror ("wait error"); 
    printf ("pid = %d\n", pid); 
#else 
    pause (); 
#endif
    printf ("parent exit\n"); 
    return 0; 
}

static void sig_cld (int signo)
{
    pid_t pid = 0; 
    int status = 0; 
    printf ("SIGCLD received\n"); 
    if (signal (SIGCLD, sig_cld) == SIG_ERR)
        perror ("signal error"); 
    if ((pid = wait (&status)) < 0)
        perror ("wait error"); 
    printf ("pid = %d\n", pid); 
}
