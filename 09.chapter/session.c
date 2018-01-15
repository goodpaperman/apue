#include "../apue.h" 
#include <sys/wait.h> 
#include <unistd.h> 

void print ()
{
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    pid_t sid = getsid (0); 
    printf ("parent %u: %u, %u\n", sid, pid, grp); 
}

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        printf ("Usage: session grpid\n"); 
        return -1; 
    }

    if (setsid() == -1)
        perror ("setsid"); 

    int ret = 0; 
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    pid_t sid = getsid (0); 
    printf ("parent %u: %u, %u\n", sid, pid, grp); 
    pid_t cid = fork (); 
    if (cid < 0)
        err_sys ("fork"); 
    else if (cid == 0)
    {
        if (setsid () == -1)
            err_sys ("setsid"); 

        pid = getpid (); 
        grp = getpgrp ();  
        sid = getsid (0); 
        printf ("session %u: %u, %u\n", sid, pid, grp); 
    }
    else 
        // parent no use
        exit (0); 

    cid = fork (); 
    if (cid < 0)
        err_sys ("fork"); 
    else if (cid == 0)
    {
        pid_t pid = getpid (); 
        pid_t grp = getpgrp ();  
        pid_t sid = getsid (0); 
        printf ("child %u: %u, %u\n", sid, pid, grp); 
        
        ret = setpgid (0, 0); 
        if (ret == -1)
            err_sys ("setpgid child"); 

        execlp(argv[1], argv[1], NULL); 
        exit (-1); 
    }
    
    // call this to ensure new group created !
    ret = setpgid (cid, cid); 
    if (ret == -1)
        err_sys ("setpgid 1"); 

    ret = setpgid (0, cid); 
    if (ret == -1)
        err_sys ("setpgid 2"); 

    print (); 
    return 0; 
}
