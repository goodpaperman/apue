#include <stdio.h> 
#include <unistd.h> 
#include <errno.h> 
#include <stdlib.h> 

int main (int argc, char *argv[])
{
    int grp = atoi (argv[1]); 
    printf ("pid %d, ppid %d, pgid %d, sid %d, tcgrp %d\n", getpid (), getppid (), getpgrp (), getsid (getpid ()), tcgetpgrp (STDIN_FILENO)); 
    sleep (1); 
    int ret = tcsetpgrp (STDIN_FILENO, grp); 
    if (ret < 0)
        printf ("tcsetpgrp failed, errno %d\n", errno); 
    else 
        printf ("pid %d, ppid %d, pgid %d, sid %d, tcgrp %d\n", getpid (), getppid (), getpgrp (), getsid (getpid ()), tcgetpgrp (STDIN_FILENO)); 
    sleep (1); 
    return 0; 
}
