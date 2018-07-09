#include "../apue.h" 
#include <sys/wait.h> 

#define CLD_NUM 2
#define USE_SIG 4

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

    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = fork ()) < 0)
            perror ("fork error"); 
        else if (pid == 0) 
        {
            sleep (3); 
            printf ("child %u exit\n", getpid ()); 
            _exit (0); 
        }

        sleep (1); 
    }

#if USE_SIG == 2
    int status = 0; 
    if ((pid = wait (&status)) < 0)
        perror ("wait error"); 

    printf ("pid = %d\n", pid); 
#elif USE_SIG == 3
    sleep (3); 
    __sighandler_t ret = signal (SIGCLD, sig_cld);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 

    int status = 0; 
    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = wait (&status)) < 0)
            perror ("wait error"); 

        printf ("pid = %d\n", pid); 
    }
#elif USE_SIG == 4
    sleep (4); 
    __sighandler_t ret = signal (SIGCLD, SIG_IGN);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 

    int status = 0; 
    for (int i=0; i<CLD_NUM; ++ i)
    {
        if ((pid = wait (&status)) < 0)
            perror ("wait error"); 

        printf ("pid = %d\n", pid); 
    }
#else
    for (int i=0; i<CLD_NUM; ++ i)
    {
        pause (); 
        printf ("wake up by signal %d\n", i); 
    }
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
