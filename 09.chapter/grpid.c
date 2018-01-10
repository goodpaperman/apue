#include "../apue.h" 

int main ()
{
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    pid_t gid1 = getpgid (pid); 
    pid_t gid2 = getpgid (0); 
    printf ("pid = %u, grp = %u\n", pid, grp); 
    printf ("gid1 = %u, gid2 = %u\n", gid1, gid2); 
    return 0; 
}
