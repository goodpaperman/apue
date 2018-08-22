#ifndef _APUE_H_
#define _APUE_H_

#define _XOPEN_SOURCE 600 /* Single Unix Specification, Version 3 */

#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/termios.h> 

#ifndef TIOCGWINSZ
#  include <sys/ioctl.h> 
#endif 

#include <stdio.h> 
#include <stdlib.h> 
#include <stddef.h> 
#include <string.h> 
#include <unistd.h> 
#include <signal.h> 
#include <fcntl.h> 
#include <signal.h> 

#define MAXLINE 4096

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH) 

typedef void Sigfunc (int); 

#if defined (SIG_IGN) && !defined (SIG_ERR)
#  define SIG_ERR((Sigfunc *) -1)
#endif 

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


void err_dump (const char *, ...); 
void err_msg (const char *, ...); 
void err_quit (const char *, ...); 
void err_exit (int, const char *, ...); 
void err_ret (const char *, ...); 
void err_sys (const char *, ...); 

char* path_alloc (int *sizep); 
long open_max (void); 


void set_fl (int fd, int flags); 
void clr_fl (int fd, int flags); 

void tell_buf (char const* name, FILE* fp); 
void pr_exit (int status); 

int apue_system (const char *cmdstring);
unsigned int apue_sleep (unsigned int sec); 

void pr_mask (sigset_t *mask); 
void pr_procmask (char const* tip); 
void pr_pendmask (char const* tip); 

//typedef void (*Sigfunc) (int signo); 
Sigfunc* apue_signal (int signo, Sigfunc *func); 
void apue_abort (); 

#define SIG2STR_MAX 10
#define APUE_SIG2STR
int sig2str (int signo, char *str);
int str2sig (const char *str, int *signop);

#endif 
