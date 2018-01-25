#include "../apue.h" 
#include <sys/wait.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/fcntl.h> 
#include <errno.h> 
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
    if (setsid() == -1)
        perror ("setsid"); 

    int ret = 0; 
    pid_t pid = getpid (); 
    pid_t grp = getpgrp ();  
    pid_t sid = getsid (0); 
    printf ("parent %u: %u, %u\n", sid, pid, grp); 

    int pty_fd = 0; 
    char name[512] = { 0 }; 
    pid_t cid = forkpty (&pty_fd, name, NULL, NULL); 
    if (cid == -1)
    {
        err_sys ("forkpty"); 
    }
    else if (cid == 0)
    {
        //pid = getpid (); 
        //grp = getpgrp ();  
        //sid = getsid (0); 
        //printf ("child before setsid %u: %u, %u\n", sid, pid, grp); 
        //if (setsid () == -1)
        //    err_sys ("setsid"); 

        pid = getpid (); 
        grp = getpgrp ();  
        sid = getsid (0); 
        printf ("child %u: %u, %u\n", sid, pid, grp); 
        
        int fd = open ("/dev/tty", O_RDWR); 
        if (fd == -1)
            err_msg ("open default tty failed, errno = %d", errno); 
        else 
            err_msg ("open default tty OK, fd = %d", fd); 

        close (fd); 
#if 0
        char *str = "hello"; 
        ret = write (pty_fd, str, strlen (str)); 
        if (ret > 0)
            printf ("send %d: %s\n", ret, str); 
#else 
        printf ("hello %d: %s\n", pty_fd, ttyname (pty_fd)); 
#endif 

        //execlp(argv[2], argv[2], NULL); 
        //char arg[128] = { 0 }; 
        //sprintf (arg, "tty=%s", ttyname (pty_fd)); 
        //execlp ("/sbin/mingetty", "mingetty", ttyname (pty_fd), NULL); 
        //execlp ("/bin/login","login", NULL); 
        execlp ("./grpid", "grpid", NULL); 
        printf ("execlp failed, errno %d\n", errno); 
        exit (-1); 
    }
    
    printf ("forkpty: %d, %s\n", pty_fd, name); 
    //char buf[1024] = "how are you ?"; 
    //write (pty_fd, buf, strlen (buf)); 
    char buf[1024] = { 0 }; 
    while (1)
    {
        ret = read (pty_fd, buf, 1024); 
        if (ret > 0)
        {
            buf[ret] = 0; 
            printf (">%s", buf); 
        }
        else 
            break; 
    }

    close (pty_fd); 
    return 0; 
}


/*
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
*/
/*
    // call this to ensure new group created !
    ret = setpgid (cid, cid); 
    if (ret == -1)
        err_sys ("setpgid 1"); 

    ret = setpgid (0, cid); 
    if (ret == -1)
        err_sys ("setpgid 2"); 

    print (); 
*/
