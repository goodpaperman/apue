#include "../apue.h" 
#include <signal.h> 
#include <errno.h> 

//#define NSIG 32

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

    return 0; 
}
