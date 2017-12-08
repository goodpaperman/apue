#include <sys/wait.h> 
#include <errno.h> 
#include <unistd.h> 
#include "../apue.h"

int my_system (const char *cmdstring)
{
    pid_t pid; 
    int status; 
    if (cmdstring == NULL)
        return 1; 

    if ((pid = fork ()) < 0)
    {
        status = -1; 
    }
    else if (pid == 0) 
    {
        printf ("before calling shell: %s\n", cmdstring); 
        execl ("/bin/sh", "sh", "-c", cmdstring, (char *)0); 
        _exit (127); 
    }
    else 
    {
        while (waitpid (pid, &status, 0) < 0)
        {
            if (errno != EINTR)
            {
                printf ("wait cmd failed, errno = %d\n", errno); 
                status = -1; 
                break; 
            }
        }
        printf ("wait cmd, status = %d\n", status); 
    }

    return (status); 
}
