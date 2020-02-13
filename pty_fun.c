
#include "pty_fun.h"
#include <errno.h> 
#include <syslog.h>
//#include <stdlib.h>
//#include <fcntl.h>

int g_verbose = 0; 
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
#elif 0 //defined (__linux__) // use system default implement
#include <sys/stat.h> // chmod
int posix_openpt (int oflag)
{
    int fdm; 
    fdm = open ("/dev/ptmx", oflag); 
    if (g_verbose)
        syslog (LOG_INFO, "open /dev/ptmx return %d\n", fdm); 

    return fdm; 
}

char *ptsname (int fdm)
{
    int sminor; 
    static char pts_name[16]; 
    if (ioctl (fdm, TIOCGPTN, &sminor) < 0)
    {
        syslog (LOG_INFO, "TIOCGPTN failed, errno %d\n", errno); 
        return NULL; 
    }

    snprintf (pts_name, sizeof (pts_name), "/dev/pts/%d", sminor); 
    if (g_verbose)
        syslog (LOG_INFO, "got ptsname %s\n", pts_name); 

    return pts_name; 
}

int grantpt (int fdm)
{
    char *pts_name; 
    pts_name = ptsname (fdm); 
    if (g_verbose)
        syslog (LOG_INFO, "grantpt for %s", pts_name); 

    return chmod (pts_name, S_IRUSR | S_IWUSR | S_IWGRP); 
}

int unlockpt (int fdm)
{
    int lock = 0; 
    if (g_verbose)
        syslog (LOG_INFO, "unimplement for unlockpt\n"); 

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

    if (grantpt (fdm) < 0) { 
        close (fdm); 
        return -2; 
    }

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
        syslog (LOG_INFO, "open default tty failed, errno = %d\n", errno); 
    else 
        syslog (LOG_INFO, "open default tty OK, tty: %s, login: %s\n", ttyname(fdtty), getlogin ()); 

    close (fdtty); 
}
  
pid_t pty_fork(int *ptrfdm, char *slave_name, int slave_namesz,  
        const struct termios *slave_termiors,  
        const struct winsize *slave_winsize, int verbose)  
{  
    int fdm = 0, fds = 0;  
    pid_t pid = 0;  
    char pts_name[20] = { 0 };  
    g_verbose = verbose; 
  
    if (verbose)
        test_tty_exist (); 

    if ((fdm = ptym_open(pts_name, sizeof(pts_name))) < 0)  {
        syslog (LOG_INFO, "can't open master pty: %s, error %d", pts_name, fdm); 
        return OPEN_PTY_ERR; 
    } 
  
    if (verbose)
        syslog (LOG_INFO, "ptym_open %d\n", fdm); 

    if (slave_name != NULL)  
    {  
        strncpy(slave_name, pts_name, slave_namesz);  
        slave_name[slave_namesz - 1] = '\0';  
    }  
  
    if ((pid = fork()) < 0)  
    {  
        syslog (LOG_INFO, "fork failed\n"); 
        return FORK_ERR;  
    }  
    else if (pid == 0)  
    {  
        if (setsid() < 0)  {
            syslog (LOG_INFO, "setsid error"); 
            return SETSID_ERR; 
        }
        else if (verbose)
            syslog (LOG_INFO, "setsid for pty_fork child ok\n"); 

        if (verbose)
            test_tty_exist (); 

        if ((fds = ptys_open(pts_name)) < 0)   {
            syslog (LOG_INFO, "can't open slave pty"); 
            return OPEN_PTYS_ERR; 
        }
        else if (verbose)
            syslog (LOG_INFO, "ptys_open %s for pty_fork child ok\n", pts_name); 

        close(fdm);  
        if (verbose)
            test_tty_exist (); 

#ifdef TIOCSCTTY   
        if (ioctl(fds, TIOCSCTTY, (char *) 0) < 0)  {
            syslog (LOG_INFO, "TIOCSCTTY error"); 
            return TIOCSCTTY_ERR; 
        }
        else if (verbose)
            syslog (LOG_INFO, "TIOCSCTTY for pty_fork child ok\n"); 
#endif   

        if (verbose)
            test_tty_exist (); 

        if (slave_termiors != NULL)   
        {   
            if (tcsetattr(fds, TCSANOW, slave_termiors) < 0)   {
                syslog (LOG_INFO, "tcsetattr error on slave pty"); 
                return INIT_ATTR_ERR; 
            }
            else if (verbose)
                syslog (LOG_INFO, "TCSANOW for pty_fork child ok\n"); 
        }   

        if (slave_winsize != NULL)   
        {   
            if (ioctl(fds, TIOCSWINSZ, slave_winsize) < 0)   {
                syslog (LOG_INFO, "TIOCSWINSZ error on slave pty"); 
                return SET_ATTR_ERR; 
            }
            else if (verbose)
                syslog (LOG_INFO, "TIOCSWINSZ for pty_fork child ok\n"); 
        }   
  
        if (dup2(fds, STDIN_FILENO) != STDIN_FILENO)  {
            syslog (LOG_INFO, "dup2 error to stdin"); 
            return DUP_STDIN_ERR;  
        }
        else if (verbose)
            syslog (LOG_INFO, "dup2 STDIN_FILENO for pty_fork child ok\n"); 

        if (dup2(fds, STDOUT_FILENO) != STDOUT_FILENO)  {
            syslog (LOG_INFO, "dup2 error to stdout"); 
            return DUP_STDOUT_ERR; 
        }
        else if (verbose)
            syslog (LOG_INFO, "dup2 STDOUT_FILENO for pty_fork child ok\n"); 

        if (dup2(fds, STDERR_FILENO) != STDERR_FILENO)  {
            syslog (LOG_INFO, "dup2 error to stderr"); 
            return DUP_STDERR_ERR; 
        }
        else if (verbose)
            syslog (LOG_INFO, "dup2 STDERR_FILENO for pty_fork child ok\n"); 

        if (fds != STDIN_FILENO && fds != STDOUT_FILENO && fds != STDERR_FILENO)  
            close(fds);  

        // chilld should return 0 just like fork
        return 0;  
    }  
    else  
    {  
        if (verbose)
            syslog (LOG_INFO, "%u fork %u ok\n", getpid (), pid); 

        *ptrfdm =fdm;  
        return pid;  
    }  
}  
