#include "../apue.h" 
#include <sys/wait.h> 

#define YUNH 500

void print ()
{
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    printf ("pid = %u, grp = %u\n", pid, grp); 
}

int main ()
{
    int ret = 0; 
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    printf ("pid = %u, grp = %u\n", pid, grp); 

    ret = setpgid (pid, grp); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    ret = setpgid (0, grp); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    //ret = setpgid (0, YUNH); 
    //if (ret == -1)
    //    err_sys ("setpgid"); 

    //print (); 
    ret = setpgid (pid, 0); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    ret = setpgid (pid, grp); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    //ret = setpgid (pid, grp+100); 
    //if (ret == -1)
    //    err_sys ("setpgid"); 

    //print (); 
    ret = setpgid (0, 0); 
    if (ret == -1)
        err_sys ("setpgid"); 

    print (); 
    return 0; 
}
