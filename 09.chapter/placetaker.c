#include <stdio.h> 
#include <unistd.h> 

int main (int argc, char *argv[])
{
    while (1)
    {
        if (getppid () == 1)
            // parent dies
            break; 

        printf ("pid %d, ppid %d, pgid %d, sid %d\n", getpid (), getppid (), getpgrp (), getsid (getpid ())); 
        sleep (1); 
    }
    
    return 0; 
}
