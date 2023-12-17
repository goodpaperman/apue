#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
#if 1
    setvbuf (stdout, NULL, _IOFBF, 0);  
    printf ("before fork\n"); 
#elif 0
    printf ("before fork "); 
#else
    printf ("before fork\n"); 
#endif

#ifdef USE_VFORK
    int pid = vfork();
#else
    int pid = fork();
#endif
    if (pid < 0)
    {
        // error
        exit(1);
    }
    else if (pid == 0)
    {
        // child
        printf ("%d spawn from %d\n", getpid(), getppid());
#ifdef USE_VFORK
        exit(0); 
#endif
    }
    else
    {
        // parent
#ifndef USE_VFOKR
        sleep (1);
#endif
        printf ("%d create %d\n", getpid(), pid);
    }

    printf ("after fork\n"); 
    return 0;
}
