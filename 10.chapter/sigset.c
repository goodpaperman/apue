#include "../apue.h" 
#include <signal.h> 
#include <errno.h> 

//#define NSIG 32

struct sigaction g_act; 
void sigint (int signo)
{
    printf ("SIGINT caught\n"); 
    printf ("mask in sigint\n"); 
    pr_mask (&g_act.sa_mask); 
    pr_procset (); 
    sleep (3); 
}

int main (int argc, char *argv[])
{
    int ret; 
    sigset_t ss; 
    printf ("sizeof(sigset_t) = %d\n", sizeof(ss)); 
    sigfillset (&ss); 
    for (int i=1; i<_NSIG; ++ i)
    {
        ret = sigismember (&ss, i); 
        if (ret == -1)
            printf ("sigismember error, %d\n", errno); 
        else if (ret)
            printf ("%d in set\n", i); 
        else 
            printf ("%d not in set\n", i); 
    }

    sigemptyset(&g_act.sa_mask); 
    sigaddset(&g_act.sa_mask, SIGQUIT); 
    g_act.sa_handler = sigint; 

    printf ("mask before call sigaction\n"); 
    pr_mask (&g_act.sa_mask); 
    pr_procset (); 
#if 1
    if(sigaction (SIGINT, &g_act, NULL) < 0)
        err_sys ("sigaction failed"); 
#else
    if (signal (SIGINT, sigint) < 0)
        err_sys ("signal failed"); 
#endif

    pause (); 
    printf ("mask after sigint called\n"); 
    pr_mask (&g_act.sa_mask); 
    pr_procset (); 
    return 0; 
}
