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
    if (argc < 3)
    {
        printf ("Usage: killsig pid signo\n"); 
        return -1; 
    }

    int signo = atoi(argv[2]); 
    signal (signo, sigint); 
    int ret = kill (atoi(argv[1]), signo); 
    printf ("kill return %d, errno %d\n", ret, errno); 
    return 0; 
}
