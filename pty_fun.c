
#include "pty_fun.h"
#include <errno.h> 

// emulate BSD system
//#define __bsdi__

#ifdef __bsdi__
#include <grp.h>
#include <fcntl.h>
#include <sys/stat.h> // chmod
int posix_openpt (int oflag)
{
    int fdm; 
    char *ptr1, *ptr2; 
    char ptm_name[16]; 
    strcpy (ptm_name, "/dev/ptyXY"); 
    for (ptr1 = "pqrstuvwxyzPQRST"; *ptr1 != 0; ptr1++) {
        ptm_name[8] = *ptr1; 
        for (ptr2 = "0123456789abcdef"; *ptr2 != 0; ptr2++) {
            ptm_name[9] = *ptr2; 
            if ((fdm = open (ptm_name, oflag)) < 0) {
                if (errno == ENOENT)
                    return -1; 
                else 
                    continue; 
            }

            return fdm; 
        }
    }

    errno = EAGAIN; 
    return -1; 
}

char* ptsname (int fdm)
{
    static char pts_name[16]; 
    char *ptm_name; 
    ptm_name = ttyname (fdm); 
    if (ptm_name == NULL)
        return NULL; 

    strncpy (pts_name, ptm_name, sizeof (pts_name)); 
    pts_name[sizeof (pts_name) - 1] = '\0'; 
    if (strncmp (pts_name, "/dev/pty/", 9) == 0)
        pts_name[9] = 's'; 
    else 
        pts_name[5] = 't'; 

    return pts_name; 
}

int grantpt (int fdm)
{
    struct group *grptr; 
    int gid; 
    char *pts_name; 

    pts_name = ptsname (fdm); 
    if ((grptr = getgrnam("tty")) != NULL)
        gid = grptr->gr_gid; 
    else 
        gid = -1; 

    if (chown (pts_name, getuid (), gid) < 0)
        return -1; 

    return chmod (pts_name, S_IRUSR | S_IWUSR | S_IWGRP); 
}

int unlockpt (int fdm) 
{
    return 0; 
}
#elif defined (__linux__)
#include <sys/stat.h> // chmod
int posix_openpt (int oflag)
{
    int fdm; 
    fdm = open ("/dev/ptmx", oflag); 
    return fdm; 
}

char *ptsname (int fdm)
{
    int sminor; 
    static char pts_name[16]; 
    if (ioctl (fdm, TIOCGPTN, &sminor) < 0)
        return NULL; 

    snprintf (pts_name, sizeof (pts_name), "/dev/pts/%d", sminor); 
    return pts_name; 
}

int grantpt (int fdm)
{
    char *pts_name; 
    pts_name = ptsname (fdm); 
    return chmod (pts_name, S_IRUSR | S_IWUSR | S_IWGRP); 
}

int unlockpt (int fdm)
{
    int lock = 0; 
    return ioctl (fdm, TIOCSPTLCK, &lock); 
}
#endif
  
int ptym_open(char *pts_name, int pts_namesz)  
{  
#if defined (__sun) || defined (sun)
	char *ptr = 0; 
	int fdm = 0; 
    strncpy (pts_name, "/dev/ptmx", pts_namesz); 
    pts_name[pts_namesz - 1] = '\0'; 
    if ((fdm = open (pts_name, O_RDWR)) < 0)
        return -1; 

#if 0
    if (grantpt (fdm) < 0) { 
        close (fdm); 
        return -2; 
    }
#endif

    if (unlockpt (fdm) <0) {
        close (fdm); 
        return -3; 
    }

    if ((ptr = ptsname (fdm)) == NULL) {
        close (fdm); 
        return -4; 
    }

    strncpy (pts_name, ptr, pts_namesz); 
    pts_name[pts_namesz - 1] = '\0'; 
    return fdm; 
#elif defined (__bsdi__) // BSD
    char *ptr; 
    int fdm; 
    strncpy (pts_name, "/dev/ptyXX", pts_namesz); 
    pts_name[pts_namesz - 1] = '\0'; 
    if ((fdm = posix_openpt (O_RDWR)) < 0)
        return -1; 

    if (grantpt (fdm) < 0) {
        close (fdm); 
        return -2; 
    }

    if (unlockpt (fdm) < 0) { 
        close (fdm); 
        return -3; 
    }

    if ((ptr = ptsname (fdm)) == NULL) { 
        close (fdm); 
        return -4; 
    }

    strncpy (pts_name, ptr, pts_namesz); 
    pts_name[pts_namesz - 1] = '\0'; 
    return fdm; 
#else // common linux
    char *ptr = 0;  
    char fdm = 0;  
    /* 
     *return the name of the master device so that on failure 
     *the caller can print an error message. Null terminate to 
     *handle case where string lenth > pts_namesz 
     * */  
  
    strncpy(pts_name, "/dev/ptmx", pts_namesz);  
    pts_name[pts_namesz - 1] = '\0';  
  
    fdm = posix_openpt(O_RDWR);  
    if (fdm < 0)  
        return OPEN_PTY_ERR;  

    if (grantpt(fdm) < 0)  
    {  
        close(fdm);  
        return GRANT_PTY_ERR;  
    }  
    if (unlockpt(fdm) < 0)  
    {  
        close(fdm);  
        return UNLOCK_PTY_ERR;  
    }  
    if ((ptr = ptsname(fdm)) == NULL)  
    {  
        close(fdm);  
        return GET_PTYS_NAME_ERR;  
    }  

    strncpy(pts_name, ptr, pts_namesz);  
    pts_name[pts_namesz - 1] = '\0';  
    return fdm;  
#endif 
}  
  
int ptys_open(char *pts_name)  
{  
#if defined (__sun) || defined (sun)
    int fds, setup; 
    if ((fds = open (pts_name, O_RDWR)) < 0)
        return -5; 

    if ((setup = ioctl (fds, I_FIND, "ldterm")) < 0) {
        close (fds); 
        return -6; 
    }

    if (setup == 0) {
        if (ioctl (fds, I_PUSH, "ptem") < 0) { 
            close (fds); 
            return -7; 
        }

        if (ioctl (fds, I_PUSH, "ldterm") < 0) {
            close (fds); 
            return -8; 
        }

        if (ioctl (fds, I_PUSH, "ttcompat") < 0) {
            close (fds); 
            return -9; 
        }
    }

    return fds; 
// same with linux
//#elif defined (__bsdi__)
//    int fds; 
//    if ((fds = open (pts_name, O_RDWR)) < 0)
//        return -5; 
//
//    return fds; 
#else 
    int fds;  
    // if open O_NOCTTY flag, open pts will not automatically 
    // set it to default tty, you must call 
    //    ioctl(fds, TIOCSCTTY, (char *) 0); 
    // exiplicitly
    if ((fds = open(pts_name, O_RDWR /*| O_NOCTTY*/)) < 0)  
        return OPEN_PTYS_ERR;  
    return fds;  
#endif
}  

void test_tty_exist ()
{
    int fdtty = open ("/dev/tty", O_RDWR); 
    if (fdtty == -1)
        printf ("open default tty failed, errno = %d\n", errno); 
    else 
        printf ("open default tty OK, tty: %s, login: %s\n", ttyname(fdtty), getlogin ()); 

    close (fdtty); 
}
  
int pty_fork(int *ptrfdm, char *slave_name, int slave_namesz,  
        const struct termios *slave_termiors,  
        const struct winsize *slave_winsize, pid_t *ppid)  
{  
    int fdm = 0, fds = 0;  
    pid_t pid = 0;  
    char pts_name[20] = { 0 };  
  
    test_tty_exist (); 
    if ((fdm = ptym_open(pts_name, sizeof(pts_name))) < 0)  
    {  
        return fdm;  
    }  
  
    if (slave_name != NULL)  
    {  
        strncpy(slave_name, pts_name, slave_namesz);  
        slave_name[slave_namesz - 1] = '\0';  
    }  
  
    if ((pid = fork()) < 0)  
    {  
        return FORK_ERR;  
    }  
    else if (pid == 0)  
    {  
        if (setsid() < 0)  
        {  
            return SETSID_ERR;  
        }  

        test_tty_exist (); 
        if ((fds = ptys_open(pts_name)) < 0)  
        {  
            close(fdm);  
            return OPEN_PTYS_ERR;  
        }  

        test_tty_exist (); 
#ifdef TIOCSCTTY   
        if (ioctl(fds, TIOCSCTTY, (char *) 0) < 0)  
            return TIOCSCTTY_ERR;  
#endif   

        test_tty_exist (); 
        if (slave_termiors != NULL)   
        {   
            if (tcsetattr(fds, TCSANOW, slave_termiors) < 0)   
                return INIT_ATTR_ERR;   
        }   
        if (slave_winsize != NULL)   
        {   
            if (ioctl(fds, TIOCSWINSZ, slave_winsize) < 0)   
                return INIT_ATTR_ERR;   
        }   
  
        if (dup2(fds, STDIN_FILENO) != STDIN_FILENO)  
            return DUP_STDIN_ERR;  
        if (dup2(fds, STDOUT_FILENO) != STDOUT_FILENO)  
            return DUP_STDOUT_ERR;  
        if (dup2(fds, STDERR_FILENO) != STDERR_FILENO)  
            return DUP_STDERR_ERR;  
        if (fds != STDIN_FILENO && fds != STDOUT_FILENO && fds != STDERR_FILENO)  
        {  
            close(fds);  
        }  

        if (ppid)
            *ppid = 0;  
        return 0;  
    }  
    else  
    {  
        *ptrfdm =fdm;  
        if (ppid)
            *ppid = pid;  
        return 0;  
    }  
}  
