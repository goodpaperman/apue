#include "apue.h"
#include <errno.h> 
#include <limits.h> 
#include <stdarg.h> 
#include <sys/wait.h> 
#include <setjmp.h>
#include <time.h> 
#include <signal.h>
#include <strings.h>
#ifdef USE_PTHREAD
#include <pthread.h> 
#endif
#include <syslog.h>
#include <fcntl.h> 
#include <poll.h>
#include <sys/sem.h> 
#include <sys/types.h>
#include <sys/socket.h> 
#include <sys/resource.h>
#include <sys/time.h>
#include <syslog.h>


static void err_doit (int, int, const char *, va_list); 

void 
err_dump (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, errno, fmt, ap); 
  va_end (ap); 
  abort (); 
  exit (1); 
}

void 
err_msg (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (0, 0, fmt, ap); 
  va_end (ap); 
}

void 
err_quit (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (0, 0, fmt, ap); 
  va_end (ap); 
  exit (1); 
}

void 
err_exit (int error, const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, error, fmt, ap); 
  va_end (ap); 
  exit (1); 
}

void 
err_ret (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, errno, fmt, ap); 
  va_end (ap); 
}

void 
err_sys (const char *fmt, ...)
{
  va_list ap; 
  va_start (ap, fmt); 
  err_doit (1, errno, fmt, ap); 
  va_end (ap); 
  exit (1); 
}

static void 
err_doit (int errnoflag, int error, const char *fmt, va_list ap)
{
  char buf [MAXLINE] = { 0 }; 
  vsnprintf (buf, MAXLINE, fmt, ap); 
  if (errnoflag)
    snprintf (buf+strlen (buf), MAXLINE-strlen (buf), ": %s", strerror (error)); 
  strcat (buf, "\n"); 
  fflush (stdout); 
  fputs (buf, stderr); 
  fflush (0); 
}

#if !defined(NO_LOG_TO_STDERR)
int log_to_stderr = 1; 
#else
extern int log_to_stderr; 
#endif

static void log_doit (int, int, const char*, va_list ap); 
void log_open (const char *ident, int option, int facility)
{
    if (log_to_stderr == 0)
        openlog (ident, option, facility); 
}

void log_ret (const char *fmt, ...)
{
    va_list ap; 
    va_start (ap, fmt); 
    log_doit (1, LOG_ERR, fmt, ap); 
    va_end (ap); 
}

void log_sys (const char *fmt, ...)
{
    va_list ap; 
    va_start (ap, fmt); 
    log_doit (1, LOG_ERR, fmt, ap); 
    va_end (ap); 
    exit (2); 
}

void log_msg (const char *fmt, ...)
{
    va_list ap; 
    va_start (ap, fmt); 
    log_doit (0, LOG_ERR, fmt, ap); 
    va_end (ap); 
}

void log_quit (const char *fmt, ...)
{
    va_list ap; 
    va_start (ap, fmt); 
    log_doit (0, LOG_ERR, fmt, ap); 
    va_end (ap); 
}

static void log_doit (int errnoflag, int priority, const char *fmt, va_list ap)
{
    int errno_save; 
    char buf[MAXLINE]; 
    errno_save = errno; 
    vsnprintf (buf, MAXLINE, fmt, ap); 
    if (errnoflag)
        snprintf (buf + strlen(buf), MAXLINE - strlen(buf), ": %s", strerror (errno_save)); 
    strcat (buf, "\n"); 
    if (log_to_stderr) { 
        fflush (stdout); 
        fputs (buf, stderr); 
        fflush (stderr); 
    } else {
        syslog (priority, buf); 
    }
}

#ifdef PATH_MAX
static int pathmax = PATH_MAX; 
#else
static int pathmax = 0; 
#endif 

#define SUSV3 200112L

static long posix_version = 0; 

#define PATH_MAX_GUESS 1024

char* path_alloc (int *sizep)
{
  char *ptr; 
  int size; 
  if (posix_version == 0)
    posix_version = sysconf (_SC_VERSION); 

  if (pathmax == 0)
  {
    errno = 0; 
    if ((pathmax = pathconf ("/", _PC_PATH_MAX)) < 0) 
    {
      if (errno == 0)
        pathmax = PATH_MAX_GUESS; 
      else 
        err_sys ("pathconf error for _PC_PATH_MAX"); 
    }
    else 
      pathmax ++; 
  }

  if (posix_version < SUSV3) 
    size = pathmax + 1; 
  else 
    size = pathmax; 

  if ((ptr = malloc (size)) == NULL)
    err_sys ("malloc error for pathname"); 

  if (sizep != NULL)
    *sizep = size; 

  return ptr; 
}

#ifdef OPEN_MAX
static int openmax = OPEN_MAX; 
#else
static int openmax = 0; 
#endif 

#define OPEN_MAX_GUESS 256

long open_max (void)
{
  if (openmax == 0) { 
    errno = 0; 
    if ((openmax = sysconf (_SC_OPEN_MAX)) < 0) { 
      if (errno == 0) 
        openmax = OPEN_MAX_GUESS; 
      else 
        err_sys ("sysconf error for _SC_OPEN_MAX"); 
    }
  }

  return openmax; 
}

void set_fl (int fd, int flags)
{
  int val = fcntl (fd, F_GETFL, 0); 
  if (val < 0)
    err_sys ("fcntl F_GETFL error"); 

  val |= flags; 
  if (fcntl (fd, F_SETFL, val) < 0)
    err_sys ("fcntl F_SETFL error"); 
}


void clr_fl (int fd, int flags)
{
  int val = fcntl (fd, F_GETFL, 0); 
  if (val < 0)
    err_sys ("fcntl F_GETFL error"); 

  val &= ~flags; 
  if (fcntl (fd, F_SETFL, val) < 0)
    err_sys ("fcntl F_SETFL error"); 
}

void tell_buf (char const* name, FILE* fp)
{
  printf ("%s is: ", name); 
#ifdef _IO_UNBUFFERED
  if (fp->_flags & _IO_UNBUFFERED)
    printf ("unbuffered\n"); 
  else
#endif
#ifdef _IO_LINE_BUF
  if (fp->_flags & _IO_LINE_BUF)
    printf ("line-buffered\n"); 
  else
#endif
    printf ("fully-buffered\n"); 

#ifdef __linux__
  printf ("buffer size is %d, %p\n", fp->_IO_buf_end - fp->_IO_buf_base, fp->_IO_buf_base); 
#endif
  printf ("discriptor is %d\n\n", fileno (fp)); 
}

#if 0
int apue_system (const char *cmdstring)
{
    pid_t pid; 
    int status; 
    if (cmdstring == NULL)
        return 1; 

    if ((pid = fork ()) < 0)
    {
        status = -1; 
    }
    else if (pid == 0) 
    {
        printf ("before calling shell: %s\n", cmdstring); 
        execl ("/bin/sh", "sh", "-c", cmdstring, (char *)0); 
        _exit (127); 
    }
    else 
    {
        //sleep (1); 
        while (waitpid (pid, &status, 0) < 0)
        {
            if (errno != EINTR)
            {
                printf ("wait cmd failed, errno = %d\n", errno); 
                status = -1; 
                break; 
            }
        }
        printf ("wait cmd, status = %d (0x%08x)\n", status, status); 
    }

    return (status); 
}
#else
int apue_system (const char *cmdstring)
{
    pid_t pid; 
    int status; 
    struct sigaction ignore, saveintr, savequit; 
    sigset_t chldmask, savemask; 
    if (cmdstring == NULL)
        return 1; 

    ignore.sa_handler = SIG_IGN; 
    sigemptyset (&ignore.sa_mask); 
    ignore.sa_flags = 0; 
    if (sigaction (SIGINT, &ignore, &saveintr) < 0)
        return -1; 
    if (sigaction (SIGQUIT, &ignore, &savequit) < 0)
        return -1; 

    sigemptyset (&chldmask); 
    sigaddset (&chldmask, SIGCHLD); 
    if (sigprocmask (SIG_BLOCK, &chldmask, &savemask) < 0)
        return -1; 

    if ((pid = fork ()) < 0)
    {
        status = -1; 
    }
    else if (pid == 0) 
    {
        sigaction (SIGINT, &saveintr, NULL); 
        sigaction (SIGQUIT, &savequit, NULL); 
        sigprocmask (SIG_SETMASK, &savemask, NULL); 

        printf ("before calling shell: %s\n", cmdstring); 
        execl ("/bin/sh", "sh", "-c", cmdstring, (char *)0); 
        _exit (127); 
    }
    else 
    {
        //sleep (10); 
        while (waitpid (pid, &status, 0) < 0)
        {
            if (errno != EINTR)
            {
                printf ("wait cmd failed, errno = %d\n", errno); 
                status = -1; 
                break; 
            }
        }
    }

    if (sigaction (SIGINT, &saveintr, NULL) < 0)
        return -1; 
    if (sigaction (SIGQUIT, &savequit, NULL) < 0)
        return -1; 
    if (sigprocmask (SIG_SETMASK, &savemask, NULL) < 0)
        return -1; 

    return (status); 
}
#endif

void pr_exit (int status)
{
  if (WIFEXITED(status))
    printf ("normal termination, exit status = %d\n", WEXITSTATUS(status)); 
  else if (WIFSIGNALED(status))
    printf ("abnormal termination, signal number = %d%s\n", 
      WTERMSIG(status), 
#  ifdef WCOREDUMP
      WCOREDUMP(status) ? " (core file generated)" : ""); 
#  else 
      ""); 
#  endif 
  else if (WIFSTOPPED(status))
    printf ("child stopped, signal number = %d\n", WSTOPSIG(status)); 
  else 
    printf ("unkown status 0x%x\n", status); 
}


#if 0
#  ifdef USEJMP 
static jmp_buf env_alrm; 
#  endif 
static void sig_alrm (int signo)
{
    printf ("recv signal %d, wakeup from sleep\n", signo); 
#  ifdef USEJMP 
    longjmp (env_alrm, 1); 
#  endif
}

unsigned int apue_sleep (unsigned int sec)
{
    time_t start = time (NULL); 
    __sighandler_t old = signal (SIGALRM, sig_alrm); 
    if (old == SIG_ERR)
        return sec; 

    int ret = 0; 
#  ifdef USEJMP
    if (setjmp (env_alrm) == 0)
#  endif
    {
        ret = alarm (sec); 
        if (ret < sec)
        {
            sec = ret;
            alarm (sec); 
        }

#  if 1
        sleep (sec); 
        printf ("start real wait\n"); 
#  endif

        pause (); 
    }

    signal (SIGALRM, old); 
    time_t end = time (NULL); 
    int dure = (int)(end - start); 
    printf ("alarm return %d, sleep %u, start %lu, end %lu\n", ret, dure, start, end); 
    int left = ret > dure ? ret - dure : 0; 
    alarm (left); 
    return left; 
}
#else
static void sig_alrm (int signo)
{
    printf ("sig alrm caught\n"); 
}

unsigned int apue_sleep (unsigned int nsecs)
{
    time_t start = time (NULL); 
    struct sigaction newact, oldact; 
    sigset_t newmask, oldmask, suspmask; 
    unsigned int unslept; 

    newact.sa_handler = sig_alrm; 
    sigemptyset (&newact.sa_mask); 
    newact.sa_flags = 0; 
    sigaction (SIGALRM, &newact, &oldact); 

    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGALRM); 
    sigprocmask (SIG_BLOCK, &newmask, &oldmask); 

#  if 0
    alarm (nsecs); 
#  else 
    int ret = alarm (nsecs); 
    if (ret < nsecs)
    {
        //nsecs = ret;
        alarm (ret); 
    }
#  endif 

    if (oldact.sa_handler != SIG_IGN && 
        oldact.sa_handler != SIG_DFL && 
        ret > nsecs)
    {
        // before suspend on SIGALRM, restore old action.
        // to call user's handler when SIGALRM called
        sigaction (SIGALRM, &oldact, NULL); 
    }

    suspmask = oldmask; 
    sigdelset (&suspmask, SIGALRM); 
    sigsuspend (&suspmask); 

    time_t end = time (NULL); 
    int dure = (int)(end - start); 
    printf ("want %u, real %u, sleep %u, start %lu, end %lu\n", nsecs, ret, dure, start, end); 
#  if 0
    unslept = alarm (0); 
#  else 
    unslept = ret > dure ? ret - dure : 0; 
    alarm (unslept); 
#  endif

    printf ("alarm return %d\n", unslept); 

    // a patch
    //if (oldact.sa_handler == SIG_IGN)
    sigaction (SIGALRM, &oldact, NULL); 

    sigprocmask (SIG_SETMASK, &oldmask, NULL); 
    return unslept; 
}
#endif 

void pr_mask (sigset_t *mask)
{
    int i; 
    char text[SIG2STR_MAX]; 
#ifndef _NSIG
#  define _NSIG 32
#endif
    for (i=1; i<_NSIG; ++ i)
    {
        if (sigismember (mask, i))
        {
#if 0
            printf ("%d\n", i); 
#else 
            sig2str (i, text); 
            printf ("%s ", text); 
#endif
        }
    }

    printf ("\n"); 
}

void pr_procmask (char const* tip)
{
    sigset_t mask; 
    if (sigprocmask (0, NULL, &mask) < 0)
        printf ("sigprocmask failed, errno %d", errno); 
    else 
    {
        //printf ("block mask contains: \n"); 
        puts (tip); 
        pr_mask (&mask); 
    }
}

#ifdef USE_PTHREAD
void pr_threadmask (char const* tip)
{
    sigset_t mask; 
    if (pthread_sigmask (0, NULL, &mask) < 0)
        printf ("pthread_sigmask failed, errno %d", errno); 
    else 
    {
        //printf ("block mask contains: \n"); 
        puts (tip); 
        pr_mask (&mask); 
    }
}
#endif 

void pr_pendmask (char const* tip)
{
    sigset_t mask; 
    if (sigpending (&mask) < 0)
        printf ("sigpending failed, errno %d", errno); 
    else 
    {
        //printf ("pend mask contains: \n"); 
        puts (tip); 
        pr_mask (&mask); 
    }
}

Sigfunc* apue_signal (int signo, Sigfunc *func)
{
    struct sigaction act, oact; 
    act.sa_handler = func; 
    sigemptyset (&act.sa_mask); 
    act.sa_flags = 0; 
    if (signo == SIGALRM) { 
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT; 
#endif 
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART; 
#endif
    }
    
    if (sigaction (signo, &act, &oact) < 0)
        return (SIG_ERR); 

    return oact.sa_handler; 
}

void apue_abort ()
{
    sigset_t mask; 
    struct sigaction action; 
    sigaction (SIGABRT, NULL, &action); 
    if (action.sa_handler == SIG_IGN)
    {
        printf ("change ignore to default\n"); 
        action.sa_handler = SIG_DFL; 
        sigaction (SIGABRT, &action, NULL); 
    }

    if (action.sa_handler == SIG_DFL)
        fflush (NULL); 

    sigfillset (&mask); 
    sigdelset (&mask, SIGABRT); 
    sigprocmask (SIG_SETMASK, &mask, NULL); 
    kill (getpid (), SIGABRT); 

    printf ("after execute user handler\n"); 
    fflush (NULL); 
    action.sa_handler = SIG_DFL; 
    sigaction (SIGABRT, &action, NULL); 
    sigprocmask (SIG_SETMASK, &mask, NULL); 
    kill (getpid (), SIGABRT); 
    printf ("never get here!"); 
    exit (1); 
}


struct key_value
{
    int key; 
    char value[SIG2STR_MAX]; 
}; 

//#define UNKNOWN 0
#define SIGDEF(name) {name, #name}

static struct key_value signames[_NSIG]=
{
    SIGDEF(0), 
    SIGDEF(SIGHUP), 
    SIGDEF(SIGINT), 
    SIGDEF(SIGQUIT), 
    SIGDEF(SIGILL), 
    SIGDEF(SIGTRAP), 
    SIGDEF(SIGABRT), 
    SIGDEF(SIGBUS), 
    SIGDEF(SIGFPE), 
    SIGDEF(SIGKILL), 
    SIGDEF(SIGUSR1), 
    SIGDEF(SIGSEGV), 
    SIGDEF(SIGUSR2), 
    SIGDEF(SIGPIPE), 
    SIGDEF(SIGALRM), 
    SIGDEF(SIGTERM), 
#ifdef SIGSTKFLT
    SIGDEF(SIGSTKFLT), 
#endif
    SIGDEF(SIGCHLD), 
    SIGDEF(SIGCONT), 
    SIGDEF(SIGSTOP), 
    SIGDEF(SIGTSTP), 
    SIGDEF(SIGTTIN), 
    SIGDEF(SIGTTOU), 
    SIGDEF(SIGURG), 
    SIGDEF(SIGXCPU), 
    SIGDEF(SIGXFSZ), 
    SIGDEF(SIGVTALRM), 
    SIGDEF(SIGPROF), 
#ifdef SIGWINCH
    SIGDEF(SIGWINCH), 
#endif
#ifdef SIGIO
    SIGDEF(SIGIO), 
#endif
#ifdef SIGPWR
    SIGDEF(SIGPWR), 
#endif
    SIGDEF(SIGSYS), 
    SIGDEF(32), 
    SIGDEF(33), 
    SIGDEF(34), 
    SIGDEF(35), 
    SIGDEF(36), 
    SIGDEF(37), 
    SIGDEF(38), 
    SIGDEF(39), 
    SIGDEF(40), 
    SIGDEF(41), 
    SIGDEF(42), 
    SIGDEF(43), 
    SIGDEF(44), 
    SIGDEF(45), 
    SIGDEF(46), 
    SIGDEF(47), 
    SIGDEF(48), 
    SIGDEF(49), 
    SIGDEF(50), 
    SIGDEF(51), 
    SIGDEF(52), 
    SIGDEF(53), 
    SIGDEF(54), 
    SIGDEF(55), 
    SIGDEF(56), 
    SIGDEF(57), 
    SIGDEF(58), 
    SIGDEF(59), 
    SIGDEF(60), 
    SIGDEF(61), 
    SIGDEF(62), 
    SIGDEF(63), 
    SIGDEF(64), 
}; 

int sig2str (int signo, char *str)
{
    if (signo < 0 || signo >= _NSIG)
    {
        return -1; 
    }

    strcpy (str, signames[signo].value); 
    return 0; 
}

int str2sig (const char *str, int *signop)
{
    int i = 0; 
    for (; i<_NSIG; ++ i)
    {
        if (strcasecmp (str, signames[i].value) == 0)
        {
            *signop = signames[i].key; 
            return 0; 
        }
    }

    return -1; 
}

void printids (char const* prompt)
{
    printf ("%s: pid %6d, ppid %6d, pgid %6d, sid %6d, tcgrp %6d\n", prompt, getpid (), getppid (), getpgrp (), getsid (getpid ()), tcgetpgrp (STDIN_FILENO)); 
    syslog (LOG_INFO, "%s: pid %6d, ppid %6d, pgid %6d, sid %6d, tcgrp %6d\n", prompt, getpid (), getppid (), getpgrp (), getsid (getpid ()), tcgetpgrp (STDIN_FILENO)); 
}

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int lockfile (int fd)
{
    struct flock fl; 
    fl.l_type = F_WRLCK; 
    fl.l_start = 0; 
    fl.l_whence = SEEK_SET; 
    fl.l_len = 0; 
    return (fcntl (fd, F_SETLK, &fl)); 
}

int already_running (void)
{
    int fd; 
    char buf[16]; 
    fd = open (LOCKFILE, O_RDWR | O_CREAT, LOCKMODE); 
    if (fd < 0) { 
        syslog (LOG_ERR, "can't open %s: %m", LOCKFILE); 
        exit (1); 
    }

    if (lockfile (fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) { 
            syslog (LOG_INFO, "%ld has running instance, (%m), exiting..", (long)getpid ()); 
            close (fd); 
            return 1; 
        }

        syslog (LOG_ERR, "can't lock %s: %m", LOCKFILE); 
        exit (1); 
    }

    ftruncate (fd, 0); 
    sprintf (buf, "%ld", (long)getpid ()); 
    write (fd, buf, strlen (buf) + 1); 
    syslog (LOG_INFO, "first instance of daemon, allow running!"); 
    return 0; 
}


void daemonize (char const* cmd)
{
    int i, fd0, fd1, fd2; 
    pid_t pid; 
    struct rlimit rl; 
    struct sigaction sa; 

    // 1st
    umask (0); 
    printids ("after umask      "); 

    if (getrlimit (RLIMIT_NOFILE, &rl) < 0)
        err_quit ("%s: can't get file limit", cmd); 

    // 2nd 
    if ((pid = fork ()) < 0)
        err_quit ("%s: can't fork", cmd); 
    else if (pid != 0) // parent
    { 
        printids ("after fork p1    "); 
        exit (0); 
    }

    printids ("after fork c1    "); 

    // 3rd 
    setsid (); 
    printids ("after setsid     "); 

    // 4th
    sa.sa_handler = SIG_IGN; 
    sigemptyset (&sa.sa_mask); 
    sa.sa_flags = 0; 
    if (sigaction (SIGHUP, &sa, NULL) < 0)
        err_quit ("%s: can't ignore SIGHUP"); 

    // 5th 
    if ((pid = fork ()) < 0)
        err_quit ("%s: can't fork", cmd); 
    else if (pid != 0) // parent
    {
        printids ("after fork p2    "); 
        exit (0); 
    }

    printids ("after fork c2    "); 

    // 6th
    if (chdir ("/") < 0)
        err_quit ("%s: can't change directory to /"); 

#if 0
    // to see chroot make openlog failure (can not find /dev/log)
    int ret = chroot ("/home/yunhai/code/apue"); 
    if (ret == -1)
        syslog (LOG_ERR, "chroot failed"); 
    else 
        syslog (LOG_INFO, "chroot to .."); 
#endif

    // 7th
    syslog (LOG_INFO, "rlimit max files: %ld\n", rl.rlim_max); 
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024; 

    syslog (LOG_INFO, "rlimit max files: %ld\n", rl.rlim_max); 
#if 1
    for (i=0; i<rl.rlim_max; ++ i)
        close (i); 

    // 8th
    fd0 = open ("/dev/null", O_RDWR); 
    fd1 = dup(0); 
    fd2 = dup(0); 

    // 9th
    openlog (cmd, LOG_CONS /*| LOG_PID*/, LOG_DAEMON); 
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog (LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2); 
        exit (1); 
    }
#else
    closelog (); 
    openlog (cmd, LOG_CONS /*| LOG_PID*/, LOG_DAEMON); 
    syslog (LOG_INFO, "after open log"); 
#endif
}

int lock_reg (int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
  struct flock lock; 
  lock.l_type = type; 
  lock.l_start = offset; 
  lock.l_whence = whence; 
  lock.l_len = len; 
  return fcntl (fd, cmd, &lock); 
}

pid_t lock_test (int fd, int type, off_t *offset, int *whence, off_t *len)
{
  struct flock lock; 
  lock.l_type = type; 
  lock.l_start = *offset; 
  lock.l_whence = *whence; 
  lock.l_len = *len; 
  if (fcntl(fd, F_GETLK, &lock) < 0)
    err_sys ("fcntl error"); 

  if (lock.l_type == F_UNLCK)
    return 0; 

  *offset = lock.l_start; 
  *whence = lock.l_whence; 
  *len = lock.l_len; 
  return lock.l_pid; 
}

#ifdef USE_FLCK_SYNC

static int fd = -1; 

void SYNC_INIT ()
{
    fd = open ("./lock_to_sync", O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
    if (fd < 0)
        err_sys ("open file to add lock on it for synchronize failed\n"); 

    int ret = write (fd, "ab", 2); 
    if (ret != 2)
        err_sys ("init lock file to length 2 failed\n"); 

    printf ("open lock file and extend to 2 bytes\n"); 

#if 0
    if (writew_lock (fd, 0, SEEK_SET, 1) < 0)
        err_sys ("try to hold lock for parent failed\n"); 

    if (writew_lock (fd, 1, SEEK_SET, 1) < 0)
        err_sys ("try to hold lock for child failed\n"); 

    printf ("lock these two bytes for init ok\n"); 
#else
    //// parent operates child lock, and child operate parent lock.
    //if (writew_lock (fd, child ? 0 : 1, SEEK_SET, 1) < 0)
    // called by parent and lock child byte
    if (writew_lock (fd, 1, SEEK_SET, 1) < 0)
        err_sys ("try to hold child lock for parent failed\n"); 

    printf ("lock child bytes for parent init ok\n"); 
#endif 
}

void SYNC_REINIT()
{
    // called by child, and lock parent byte
    if (writew_lock (fd, 0, SEEK_SET, 1) < 0)
        err_sys ("try to hold parent lock for child failed\n"); 

    printf ("lock parent bytes for child init ok\n"); 
}

void SYNC_TELL (pid_t pid, int child)
{
    int offset = child ? 1 : 0; 
    if (un_lock(fd, offset, SEEK_SET, 1) < 0)
        err_sys ("release lock for %s failed\n", child ? "child" : "parent"); 

    // in reverse turn
    printf ("%s unlock file at %d\n", child ? "parent" : "child", offset); 
}

void SYNC_WAIT (int child)
{
    int offset = child ? 1 : 0; 
    if (writew_lock (fd, offset, SEEK_SET, 1) < 0)
        err_sys ("try to hold lock for %s failed\n", child ? "child" : "parent"); 

    printf ("%s lock file at %d\n", child ? "child" : "parent", offset); 
}

#else 

void SYNC_REINIT()
{
    // do nothing
}

#endif

#ifdef USE_SEM_SYNC

union semun
{
  int val;				//<= value for SETVAL
  struct semid_ds *buf; //		<= buffer for IPC_STAT & IPC_SET
  unsigned short int *array;// 		<= array for GETALL & SETALL
  struct seminfo *__buf;    //		<= buffer for IPC_INFO
};

static int semid = -1;  

void SYNC_INIT ()
{
    int mode = 0666;  // 0; 
    int flag = IPC_CREAT; 
#ifdef USE_EXCL
    flag |= IPC_EXCL; 
#endif

    semid = semget (IPC_PRIVATE, 2, flag | mode); 
    if (semid < 0)
        err_sys ("semget for SYNC failed"); 

    printf ("create semaphore %d for SYNC ok\n", semid); 

    union semun sem; 
    //sem.val = 1;
    //int ret = semctl (semid, 0, SETVAL, sem); 
    //if (ret < 0)
    //    err_sys ("semctl to set val failed"); 
    
    short arr[2] = { 0 }; 
    sem.array = arr; 
    int ret = semctl (semid, 0, SETALL, sem); 
    if (ret < 0)
        err_sys ("semctl to set all val failed"); 

    printf ("reset all semaphores ok\n"); 
}

void SYNC_TELL (pid_t pid, int child)
{
    struct sembuf sb; 
    sb.sem_op = 1; 
    sb.sem_num = child ? 1 : 0; 
    sb.sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
    int ret = semop (semid, &sb, 1); 
    if (ret < 0)
        printf ("semop to release resource failed, ret %d, errno %d\n", ret, errno); 
    else 
        printf ("release %d resource %d\n", sb.sem_op, ret); 
}

void SYNC_WAIT (int child)
{
    struct sembuf sb; 
    sb.sem_op = -1; 
    sb.sem_num = child ? 1 : 0; 
    sb.sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
    int ret = semop (semid, &sb, 1); 
    if (ret < 0)
        printf ("semop to require resource failed, ret %d, errno %d\n", ret, errno); 
    else 
        printf ("require %d resource %d\n", sb.sem_op, ret); 
}

#endif

#ifdef USE_PIPE_SYNC

// pp is the pipe that parent notify(write) child wait(read)
// pc is the pipe that child notify(write) parent wait(read)
static int pp[2], pc[2]; 

void SYNC_INIT (void)
{
    if (pipe (pp) < 0 || pipe(pc) < 0)
        err_sys ("pipe error"); 
}

void SYNC_TELL (pid_t pid, int child)
{
    // close unused read end to avoid poll receive events
    // note, we can NOT do it in SYNC_INIT, 
    // as at that moment, we have not fork yet !
    if (child) { 
        close (pp[0]); 
        close (pc[1]); 
        pp[0] = pc[1] = -1; 
    } else { 
        close (pc[0]); 
        close (pp[1]); 
        pc[0] = pp[1] = -1; 
    }

    if (write (child ? pp[1] : pc[1], child ? "p" : "c", 1) != 1)
        err_sys ("write error"); 
}

void SYNC_WAIT (int child /* unused */)
{
    int n = 0, m = 0; 
    struct pollfd fds[2] = {{ 0 }}; 
    // if fd==-1, just be a place taker !
    //if (pp[0] != -1) 
    {
        fds[n].fd = pp[0]; 
        fds[n].events = POLLIN; 
        n++; 
    }

    //if (pc[0] != -1) 
    { 
        fds[n].fd = pc[0]; 
        fds[n].events = POLLIN; 
        n++; 
    }

    int ret = poll (fds, n, -1); 
    if (ret == -1)
        err_sys ("poll error"); 
    else if (ret > 0) { 
        char c = 0; 
        //printf ("poll %d from %d\n", ret, n); 
        for (m=0; m<n; ++m) {
            //printf ("poll fd %d event 0x%08x\n", fds[m].fd, fds[m].revents); 
            if (fds[m].revents & POLLIN) { 
                if (fds[m].fd == pp[0]) { 
                    if (read (pp[0], &c, 1) != 1)
                        err_sys ("read parent pipe error"); 
                    if (c != 'p')
                        err_quit ("wait parent pipe but got incorrect data %c", c); 
                }
                else {
                    if (read (pc[0], &c, 1) != 1) 
                        err_sys ("read child pipe error"); 
                    if (c != 'c') 
                        err_quit ("wait child pipe but got incorrect data %c", c); 
                }
            }
        }
    }
    else 
        printf ("poll return 0\n"); 
}

#endif

// use signal
#ifdef USE_SIGNAL_SYNC

static volatile sig_atomic_t sigflag; 
static sigset_t newmask, oldmask, zeromask; 

static void sig_usr (int signo)
{
    sigflag = 1; 
    printf ("SIGUSR1/2 called\n"); 
}

void SYNC_INIT ()
{
    if (apue_signal (SIGUSR1, sig_usr) == SIG_ERR)
        err_sys ("signal (SIGUSR1) error"); 
    if (apue_signal (SIGUSR2, sig_usr) == SIG_ERR)
        err_sys ("signal (SIGUSR2) error"); 

    sigemptyset (&zeromask); 
    sigemptyset (&newmask); 
    sigaddset (&newmask, SIGUSR1); 
    sigaddset (&newmask, SIGUSR2); 

    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys ("SIG_BLOCK error"); 
}

void SYNC_TELL (pid_t pid, int child)
{
    kill (pid, child ? SIGUSR1 : SIGUSR2); 
}

void SYNC_WAIT (int child /* unused */)
{
    while (sigflag == 0)
        sigsuspend (&zeromask); 

    sigflag = 0; 
    if (sigprocmask (SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys ("SIG_SETMASK error"); 
}

#endif

ssize_t readn (int fd, void *ptr, size_t n)
{
    size_t nleft; 
    ssize_t nread; 
    nleft = n; 
    while (nleft > 0) { 
        nread = read (fd, ptr, nleft); 
        if (nread < 0) { 
            if (nleft == n)
                return -1; 
            else 
                break; 
        } else if (nread == 0) {
            break; // EOF
        }

        nleft -= nread; 
        ptr += nread; 
    }

    return n - nleft; 
}

ssize_t writen (int fd, const void *ptr, size_t n) 
{
    size_t nleft; 
    ssize_t nwritten; 
    nleft = n; 
    while (nleft > 0) { 
        nwritten = write (fd, ptr, nleft); 
        if (nwritten < 0) { 
            if (nleft == n)
                return -1; 
            else 
                break; 
        }
        else if (nwritten == 0) {
            break; 
        }

        nleft -= nwritten; 
        ptr += nwritten; 
    }

    return n - nleft; 
}

static pid_t *apue_popen_cid = NULL; 
static void dump_popen_cids (char const* prompt, int size)
{
    int i; 
    fprintf (stderr, "%s (%d)\n", prompt, size); 
    for (i=0; i<size; ++ i)
        if (apue_popen_cid[i] > 0)
            fprintf (stderr, "%d %d\n", i, apue_popen_cid[i]); 
    //fprintf (stderr, "\n"); 
}

FILE* apue_popen (char const* cmdstr, char const* type)
{
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) { 
        errno = EINVAL; 
        return NULL; 
    }

    int maxfd = open_max (); 
    if (apue_popen_cid == NULL) { 
        apue_popen_cid = calloc (maxfd, sizeof (pid_t)); 
        if (apue_popen_cid == NULL)
            return NULL; 
    }

    int pfd[2] = { 0 }; 
    if (pipe (pfd) < 0)
        return NULL; 

    pid_t pid = fork (); 
    if (pid < 0) 
        return NULL; 
    else if (pid == 0) { 
        // child process
        if (*type == 'r') { 
            close (pfd[0]); 
            if (pfd[1] != STDOUT_FILENO) { 
                dup2 (pfd[1], STDOUT_FILENO); 
                close (pfd[1]); 
            }
        } else { 
            close (pfd[1]); 
            if (pfd[0] != STDIN_FILENO) { 
                dup2 (pfd[0], STDIN_FILENO); 
                close (pfd[0]); 
            }
        }

        int i; 
#if 0
        sleep (1); 
#endif 
        dump_popen_cids ("before clear unused pipe handle in child", maxfd); 
        // close pipes prevous apue_popen uses
        for (i=0; i<maxfd; ++ i) {
            if (apue_popen_cid [i] > 0) { 
                close (i); 
                apue_popen_cid[i] = 0; 
            }
        }

        dump_popen_cids ("after clear unused pipe handle in child", maxfd); 
        execl ("/bin/sh", "sh", "-c", cmdstr, (char *)0); 
        _exit (127); 
    }

    // parent process
    FILE *fp = NULL; 
    if (*type == 'r') { 
        close (pfd[1]); 
        fp = fdopen (pfd[0], type); 
    } else { 
        close (pfd[0]); 
        fp = fdopen (pfd[1], type); 
    }

    if (fp == NULL)
        return NULL; 

    // set global variable after fork, 
    // child can NOT see !!
    apue_popen_cid [fileno(fp)] = pid; 
    dump_popen_cids ("after set pid in parent", maxfd); 
    return fp; 
}

int apue_pclose (FILE *fp) 
{
    if (apue_popen_cid == NULL) { 
        errno = EINVAL; 
        return -1; 
    }

    int fd = fileno (fp); 
    pid_t pid = apue_popen_cid [fd]; 
    if (pid == 0) { 
        errno = EINVAL; 
        return -1; 
    }

#if 0
    sleep (2); 
#endif

    apue_popen_cid [fd] = 0; 
    dump_popen_cids ("after clear pid in parent", open_max ()); 
    // note: must close pipe before wait child process, 
    // this is a kind of signal that tell child exit...
    if (fclose (fp) == EOF)
        return -1; 

    int stat = 0; 
    while (waitpid (pid, &stat, 0) < 0)
        if (errno != EINTR)
            return -1; 

    return stat; 

}

void print_lg_sockopt (int fd, char const* optstr, int option)
{
    struct linger val; 
    socklen_t len = sizeof (val); 
    int ret = getsockopt (fd, SOL_SOCKET, option, &val, &len); 
    if (ret < 0)
    {
        err_ret ("getsockopt for %s failed, errno %d", optstr, errno); 
        return; 
    }

#ifdef USE_DAEMON
    syslog (LOG_INFO, "%s [%d]: %d, %d", optstr, len, val.l_onoff, val.l_linger); 
#else
    printf ("%s [%d]: %d, %d\n", optstr, len, val.l_onoff, val.l_linger); 
#endif
}

void print_tm_sockopt (int fd, char const* optstr, int option)
{
    struct timeval val; 
    socklen_t len = sizeof (val); 
    int ret = getsockopt (fd, SOL_SOCKET, option, &val, &len); 
    if (ret < 0)
    {
        err_ret ("getsockopt for %s failed, errno %d", optstr, errno); 
        return; 
    }

#ifdef USE_DAEMON
    syslog (LOG_INFO, "%s [%d]: %d, %d", optstr, len, val.tv_sec, val.tv_usec); 
#else
    printf ("%s [%d]: %d, %d\n", optstr, len, val.tv_sec, val.tv_usec); 
#endif
}

void print_int_sockopt (int fd, char const* optstr, int option)
{
    int val = 0; 
    socklen_t len = sizeof (val); 
    int ret = getsockopt (fd, SOL_SOCKET, option, &val, &len); 
    if (ret < 0)
    {
        err_ret ("getsockopt for %s failed, errno %d", optstr, errno); 
        return; 
    }

#ifdef USE_DAEMON
    syslog (LOG_INFO, "%s [%d]: %d", optstr, len, val); 
#else
    printf ("%s [%d]: %d\n", optstr, len, val); 
#endif
}

void print_sockopt (int fd, char const* prompt)
{
#ifdef USE_DAEMON
    syslog (LOG_INFO, "%s options:", prompt); 
#else
    printf ("%s options:\n", prompt); 
#endif

    print_int_sockopt (fd, "SO_ACCEPTCONN", SO_ACCEPTCONN); 
    print_int_sockopt (fd, "SO_BROADCAST", SO_BROADCAST); 
    print_int_sockopt (fd, "SO_DEBUG", SO_DEBUG); 
    print_int_sockopt (fd, "SO_DONTROUTE", SO_DONTROUTE); 
    print_int_sockopt (fd, "SO_ERROR", SO_ERROR); 
    print_int_sockopt (fd, "SO_KEEPALIVE", SO_KEEPALIVE); 
    print_lg_sockopt (fd, "SO_LINGER", SO_LINGER); 
    print_int_sockopt (fd, "SO_OOBINLINE", SO_OOBINLINE); 
    print_int_sockopt (fd, "SO_RCVBUF", SO_RCVBUF); 
    print_int_sockopt (fd, "SO_RCVLOWAT", SO_RCVLOWAT); 
    print_tm_sockopt (fd, "SO_RCVTIMEO", SO_RCVTIMEO); 
    print_int_sockopt (fd, "SO_REUSEADDR", SO_REUSEADDR); 
    print_int_sockopt (fd, "SO_SNDBUF", SO_SNDBUF); 
    print_int_sockopt (fd, "SO_SNDLOWAT", SO_SNDLOWAT); 
    print_tm_sockopt (fd, "SO_SNDTIMEO", SO_SNDTIMEO); 
    print_int_sockopt (fd, "SO_TYPE", SO_TYPE); 

    int owner = fcntl (fd, F_GETOWN, 0); 
#ifdef USE_DAEMON
    syslog (LOG_INFO, "socket owner %d", owner); 
#else
    printf ("socket owner %d\n", owner); 
#endif

    int flags = fcntl (fd, F_GETFL); 
#ifdef USE_DAEMON
    syslog (LOG_INFO, "socket flags 0x%08x", flags); 
#else
    printf ("socket flags 0x%08x\n", flags); 
#endif

}

