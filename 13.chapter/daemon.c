#include "../apue.h" 
#include <syslog.h> 
#include <sys/resource.h> 
#include <limits.h> 
#include <stdarg.h>
#include <errno.h>

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
    if(already_running ())
        return 1; 

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

    int ret = 0; 
    char dir[PATH_MAX] = { 0 }; 
    getwd (dir); 
    syslog (LOG_INFO, "working dir: %s\n", dir); 

#if 0
    ret = chroot ("/home/yunhai/code/apue"); 
    if (ret == -1)
        syslog (LOG_ERR, "chroot failed"); 
    else 
        syslog (LOG_INFO, "chroot to .."); 

    getwd (dir); 
    syslog (LOG_INFO, "working dir: %s\n", dir); 
#endif

    sleep (20); 
    closelog (); 
    return 0; 
}
