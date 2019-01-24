#include "../apue.h" 
#include <syslog.h> 
#include <sys/resource.h> 
#include <limits.h> 
#include <stdarg.h>
#include <errno.h>

int _quit = 0; 

void reread (void)
{
    char const* file = "/etc/daemon.conf"; 
    syslog (LOG_INFO, "re-reading..."); 
    FILE* fp = fopen (file, "r"); 
    if (fp)
    {
        char line[4096] = { 0 }; 
        //int ret = fread (line, sizeof(line)-1, 1, fp); 
        char *ptr = fgets (line, sizeof(line)-1, fp); 
        if (ptr == NULL)
            syslog (LOG_ERR, "read conf file %s error %m", file); 
        else 
            syslog (LOG_INFO, "read: %s", line); 

        fclose (fp); 
    }
    else 
        syslog (LOG_ERR, "open conf file %s error %m", file); 

    syslog (LOG_INFO, "re-reading end"); 
}

void sigterm (int signo)
{
    syslog (LOG_INFO, "got SIGTERM; exiting"); 
    //exit (0); 
    _quit = 1; 
}

void sighup (int signo)
{
    reread (); 
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
    daemonize ("haihai"); 
    printids ("after daemonize  "); 
    if(already_running ())
        return 1; 

    struct sigaction sa; 
    sa.sa_handler = sigterm; 
    sigemptyset (&sa.sa_mask); 
    // block SIGHUP when catch SIGTERM
    sigaddset (&sa.sa_mask, SIGHUP); 
    sa.sa_flags = 0; 
    if (sigaction (SIGTERM, &sa, NULL) < 0)
    {
        syslog (LOG_ERR, "can't catch SIGTERM: %s", strerror (errno)); 
        exit (1); 
    }

    sa.sa_handler = sighup; 
    sigemptyset (&sa.sa_mask); 
    // block SIGTERM when catch SIGHUP
    sigaddset (&sa.sa_mask, SIGTERM); 
    sa.sa_flags = 0; 
    if (sigaction (SIGHUP, &sa, NULL) < 0)
    {
        syslog (LOG_ERR, "can't catch SIGHUP: %s", strerror (errno)); 
        exit (1); 
    }

    while (!_quit)
        sleep (2); 

    syslog (LOG_INFO, "daemon exit"); 
    closelog (); 
    return 0; 
}
