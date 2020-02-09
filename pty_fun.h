#ifndef PTY_FUN_H
#define PTY_FUN_H

#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   
#include <unistd.h>   
#include <sys/types.h>   
#if defined(__sun) || defined(sun)
#include <stropts.h>
#else
#include <linux/limits.h>   
#include <features.h>   
#endif

#include <termios.h>   
#include <sys/ioctl.h>   
#include <fcntl.h>   
  
//master pty   
#ifndef OPEN_PTY_ERR   
#define OPEN_PTY_ERR -1   
#endif   
  
#ifndef GRANT_PTY_ERR   
#define GRANT_PTY_ERR -2   
#endif   
  
#ifndef UNLOCK_PTY_ERR   
#define UNLOCK_PTY_ERR -3   
#endif   
  
#ifndef GET_PTYS_NAME_ERR   
#define GET_PTYS_NAME_ERR -4   
#endif   
  
//pty slave   
#ifndef OPEN_PTYS_ERR   
#define OPEN_PTYS_ERR -5   
#endif   
  
#ifndef FORK_ERR   
#define FORK_ERR -6   
#endif   
  
#ifndef SETSID_ERR   
#define SETSID_ERR -7   
#endif   
  
#ifndef TIOCSCTTY_ERR   
#define TIOCSCTTY_ERR -8   
#endif   
  
#ifndef INIT_ATTR_ERR   
#define INIT_ATTR_ERR -9   
#endif   
  
#ifndef DUP_STDIN_ERR   
#define DUP_STDIN_ERR -10   
#endif   
  
#ifndef DUP_STDOUT_ERR   
#define DUP_STDOUT_ERR -11   
#endif   
  
#ifndef DUP_STDERR_ERR   
#define DUP_STDERR_ERR -12   
#endif   
  
#include <sys/types.h>  
  
void test_tty_exist (); 

int ptym_open(char *pts_name, int pts_namesz);  
  
int ptys_open(char *pts_name);  
  
int pty_fork(int *ptrfdm, char *slave_name, int slave_namesz,  
        const struct termios *slave_termiors,  
        const struct winsize *slave_winsize, pid_t *ppid);  

#endif 
