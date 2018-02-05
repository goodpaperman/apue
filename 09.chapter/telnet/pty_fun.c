
#include "pty_fun.h"
#include <errno.h> 
  
int ptym_open(char *pts_name, int pts_namesz)  
{  
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
  
}  
  
int ptys_open(char *pts_name)  
{  
    int fds;  
    if ((fds = open(pts_name, O_RDWR)) < 0)  
        return OPEN_PTYS_ERR;  
    return fds;  
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
//      if (slave_termiors != NULL)   
//      {   
//          if (tcsetattr(fds, TCSANOW, slave_termiors) < 0)   
//              return INIT_ATTR_ERR;   
//      }   
//      if (slave_winsize != NULL)   
//      {   
//          if (ioctl(fds, TIOCSWINSZ, slave_winsize) < 0)   
//              return INIT_ATTR_ERR;   
//      }   
  
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

        *ppid = 0;  
        return 0;  
    }  
    else  
    {  
        *ptrfdm =fdm;  
        *ppid = pid;  
        return 0;  
    }  
}  
