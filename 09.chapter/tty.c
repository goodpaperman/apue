#include "../apue.h" 
#include <sys/wait.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/fcntl.h> 
#include <pty.h> 

void print ()
{
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    pid_t sid = getsid (0); 
    printf ("parent %u: %u, %u\n", sid, pid, grp); 
}

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        printf ("Usage: tty ttyname grpid\n"); 
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

    int fd = open (argv[1], O_RDWR); 
    if (fd == -1)
        err_sys ("open tty"); 

    printf ("open tty %d\n", fd); 
    //int fd2 = open ("/dev/tty", O_RDWR); 
    //if (fd2 == -1)
    //    err_sys ("open default tty"); 

    ////close (fd); 
    //close (fd2); 

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

        execlp(argv[2], argv[2], NULL); 
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
