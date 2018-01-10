#include "../apue.h" 
#include <sys/wait.h> 

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        printf ("Usage: grpmaste slaver\n"); 
        return -1; 
    }

    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    printf ("pid = %u, grp = %u\n", pid, grp); 
    pid_t cid = fork (); 
    if (cid < 0)
        err_sys ("fork"); 
    else if (cid == 0)
    {
        // child
#ifdef USE_SYSTEM
        system (argv[1]); 
#else
        execlp (argv[1], argv[1], NULL); 
#endif 

        exit (-1); 
    }

    int status = 0; 
    wait (&status); 

    pid = getpid (); 
    grp = getpgrp ();  
    printf ("pid = %u, grp = %u\n", pid, grp); 
    return 0; 
}
