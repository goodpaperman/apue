#include "../apue.h" 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h> 

void sig_handler (int signum)
{
    printf ("recv signal %d\n", signum); 
}

int main ()
{
    struct sigaction sigact; 
    sigact.sa_handler = sig_handler; 
    //SIGEMPTYSET(sigact.sa_mask); 
    //SIGADDSET(sigact.sa_mask, SIGINT); 
    sigact.sa_flags = SA_RESTART; 
    sigaction (SIGINT, &sigact, NULL); 

    while (1)
    {
#if 0
        sleep (5); 
        printf ("wait 5 over, errno = %d\n", errno); 
#elif 0
        char buf[512] = { 0 }; 
        int n = read (STDIN_FILENO, buf, sizeof(buf)-1); 
        if (n == 0 || n < 0)
            printf ("read error %d\n", errno); 
        else 
            printf ("read %s\n", buf); 
#elif 1
        pid_t pid = fork (); 
        if (pid < 0)
            printf ("fork error %d\n", errno); 
        else if (pid == 0)
        {
            int n = 0; 
            while (n++ < 3)
            {
                pause (); 
                printf ("child wakeup %d\n", n); 
            }

            printf ("child %u will die\n", getpid ()); 
            break; 
        }
        else 
        {
            int status = 0; 
            pid_t cid = wait(&status); 
            printf ("wait child %u, status %d, errno %d\n", cid, status, errno); 
            break; 
        }
#endif 
    }

    return 0; 
}
