#include "../apue.h" 
#include <sys/wait.h> 

#define YUNH 500

void print ()
{
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    printf ("child pid = %u, grp = %u\n", pid, grp); 
}

int main ()
{
    int ret = 0; 
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    printf ("child pid = %u, grp = %u\n", pid, grp); 

    ret = setpgid (0, 0); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    while (1)
    {
        if (getppid () == 1)
        {
            printf ("adopt to init\n"); 
            break;
        }

        sleep (1); 
    }
    ret = setpgid (0, grp); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    return 0; 
}
