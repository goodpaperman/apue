#include "apue.h"
#include <errno.h> 
#include <limits.h> 
#include <stdarg.h> 
#include <sys/wait.h> 
#include <setjmp.h>
#include <time.h> 
#include <signal.h>
#include <strings.h>


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
  if (fp->_flags & _IO_UNBUFFERED)
    printf ("unbuffered\n"); 
  else if (fp->_flags & _IO_LINE_BUF)
    printf ("line-buffered\n"); 
  else 
    printf ("fully-buffered\n"); 

  printf ("buffer size is %d\n", fp->_IO_buf_end - fp->_IO_buf_base); 
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
    for (i=1; i<_NSIG; ++ i)
        if (sigismember (mask, i))
            printf ("%d\n", i); 

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
    SIGDEF(SIGSTKFLT), 
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
    SIGDEF(SIGWINCH), 
    SIGDEF(SIGIO), 
    SIGDEF(SIGPWR), 
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
