#include "../apue.h" 
#include <errno.h>
#include <signal.h>

void sigint (int signo)
{
    printf ("caught %d\n", signo); 
    signal (signo, sigint); 
}

int main (int argc, char *argv[])
{
    int ret = 0; 
    pid_t pid = -1; 
    int signo = 0; 
    if (argc < 2)
    {
        printf ("Usage: killsig pid signo\n"); 
        return -1; 
    }
    else if (argc == 2)
    {
        pid = 0; 
        signo = atoi (argv[1]); 
    }
    else 
    {
        pid = atoi(argv[1]); 
        signo = atoi(argv[2]); 
    }

    signal (signo, sigint); 
#if 0
    if (argc == 2)
        ret = raise (signo); 
    else 
#endif
        ret = kill (pid, signo); 

    printf ("kill/raise return %d, errno %d\n", ret, errno); 
    return 0; 
}
