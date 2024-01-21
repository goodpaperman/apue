#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    int pid = fork();
    if (pid < 0)
    {
        // error
        exit(1);
    }
    else if (pid == 0)
    {
        // child
        printf ("%d spawn from %d\n", getpid(), getppid());
        pid = fork();
        if (pid < 0)
        {
            // error
            exit(1);
        }
        else if (pid == 0)
        {
            // child
            printf ("%d spawn from %d\n", getpid(), getppid());
            sleep (10); 
        }
        else
        {
            printf ("%d create %d\n", getpid(), pid);
        }
    }
    else
    {
        printf ("%d create %d\n", getpid(), pid);
        sleep (10); 
    }

    printf ("after fork\n"); 
    return 0;
}
