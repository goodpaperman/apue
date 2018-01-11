#include "../apue.h" 
#include <sys/wait.h> 

void print ()
{
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    printf ("parent %u: %u\n", pid, grp); 
}

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
    printf ("parent %u: %u\n", pid, grp); 
    pid_t cid = fork (); 
    if (cid < 0)
        err_sys ("fork"); 
    else if (cid == 0)
    {
        // child
        //cid = getpid (); 
        //printf ("child cid = %u\n", cid); 
        //ret = setpgid (cid, cid); 
        ret = setpgid (0, 0); 
        if (ret == -1)
            err_sys ("setpgid"); 

#ifdef USE_SYSTEM
        system (argv[1]); 
#else
        execlp(argv[1], argv[1], NULL); 
#endif 
        exit (-1); 
    }

    //printf ("parent cid = %u\n", cid); 
    ret = setpgid (cid, cid); 
    if (ret == -1)
        err_sys ("setpgid"); 

    ret = setpgid (0, cid); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    int status = 0; 
    wait (&status); 
    usleep (100000); 

    while (1)
    {
        ret = setpgid (0, 0); 
        if (ret == -1)
            err_sys ("setpgid"); 

        print (); 
        //usleep (100000); 
        ret = setpgid (0, cid); 
        if (ret == -1)
            err_sys ("setpgid"); 

        print (); 
        usleep (100000); 
    }
    return 0; 
}
