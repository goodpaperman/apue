#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>

int child_func(void *arg)
{
    // child
    printf ("%d spawned from %d\n", getpid(), getppid());
#if 0
    fclose (stdin); 
    fclose (stdout); 
    fclose (stderr); 
#else
    close (STDIN_FILENO); 
    close (STDOUT_FILENO); 
    close (STDERR_FILENO); 
#endif
    return 1; 
}

int main()
{
    printf ("before fork\n"); 

    size_t stack_size = 1024 * 1024; 
    char *stack = (char *)malloc (stack_size); 
    int pid = clone(child_func, stack+stack_size, CLONE_VM | CLONE_VFORK | CLONE_FILES | SIGCHLD, 0); 
    if (pid < 0)
    {
        // error
        exit(1);
    }

    // parent
    printf ("[1] %d create %d\n", getpid(), pid);

    char buf[128] = { 0 };  
    sprintf (buf, "[2] %d create %d\n", getpid(), pid);
    write (STDOUT_FILENO, buf, strlen(buf)); 
    return 0;
}
