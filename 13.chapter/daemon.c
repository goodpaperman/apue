#include "../apue.h" 
#include <syslog.h> 
#include <fcntl.h> 
#include <sys/resource.h> 
#include <limits.h> 
#include <stdarg.h>


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

    // 7th
    syslog (LOG_INFO, "rlimit max files: %ld\n", rl.rlim_max); 
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024; 

    syslog (LOG_INFO, "rlimit max files: %ld\n", rl.rlim_max); 
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
}

void dump (char *format, ...)
{
    va_list st; 
    va_start (st, format); 
    vsyslog (LOG_INFO, format, st); 
    va_end (st); 
}

int main (int argc, char *argv[])
{
    printids ("before daemonize "); 
    daemonize ("yunhai"); 
    printids ("after daemonize  "); 

    int n = 0; 
    for (; n < 10; ++ n)
    {
#if 1
#  if 0
        dump ("%s %s %s", "hello", "123", "nihao"); 
#  else
        dump ("%d: %s %s %s", n, "hello", "123", "nihao"); 
#  endif 
#else 
        syslog (LOG_INFO, "how are you?"); 
#endif 
    }

#if 0
    int mask = setlogmask (LOG_MASK(LOG_NOTICE)); 
    // use 0 as log nothing seems not work !
    //int mask = setlogmask (0); 
    syslog (LOG_NOTICE, "test %m, old mask 0x%x\n", mask); 
#endif 
    syslog (LOG_INFO, "umask: 0x%x\n", umask (0)); 

    char dir[PATH_MAX] = { 0 }; 
    getwd (dir); 

    syslog (LOG_INFO, "working dir: %s\n", dir); 
    closelog (); 
    return 0; 
}
