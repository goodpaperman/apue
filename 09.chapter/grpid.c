#include "../apue.h" 

int main ()
{
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    pid_t sid = getsid (0); 
    pid_t gid1 = getpgid (pid); 
    pid_t gid2 = getpgid (0); 
    printf ("sid = %u, pid = %u, grp = %u\n", sid, pid, grp); 
    printf ("gid1 = %u, gid2 = %u\n", gid1, gid2); 
    return 0; 
}
