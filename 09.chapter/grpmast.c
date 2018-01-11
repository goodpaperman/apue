#include "../apue.h" 
#include <sys/wait.h> 

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        printf ("Usage: grpmast slave\n"); 
        return -1; 
    }

    int ret = 0; 
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    printf ("parent pid = %u, grp = %u\n", pid, grp); 
    pid_t cid = fork (); 
    if (cid < 0)
        err_sys ("fork"); 
    else if (cid == 0)
    {
        // child
        cid = getpid (); 
        printf ("child cid = %u\n", cid); 
        ret = setpgid (cid, cid); 
        if (ret == -1)
            err_sys ("setpgid"); 

#ifdef USE_SYSTEM
        system (argv[1]); 
#else
        execlp(argv[1], argv[1], NULL); 
#endif 
        exit (-1); 
    }

    printf ("parent cid = %u\n", cid); 
    ret = setpgid (cid, cid); 
    if (ret == -1)
        err_sys ("setpgid"); 

    ret = setpgid (0, cid); 
    if (ret == -1)
        err_sys ("setpgid"); 

    int status = 0; 
    wait (&status); 

    pid = getpid (); 
    grp = getpgrp ();  
    printf ("parent pid = %u, grp = %u\n", pid, grp); 

    ret = setpgid (0, 0); 
    if (ret == -1)
        err_sys ("setpgid"); 

    pid = getpid (); 
    grp = getpgrp ();  
    printf ("parent pid = %u, grp = %u\n", pid, grp); 

    ret = setpgid (0, cid); 
    if (ret == -1)
        err_sys ("setpgid"); 

    pid = getpid (); 
    grp = getpgrp ();  
    printf ("parent pid = %u, grp = %u\n", pid, grp); 
    return 0; 
}
