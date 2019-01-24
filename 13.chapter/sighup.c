#include "../apue.h" 
#include <syslog.h> 
#include <sys/resource.h> 
#include <limits.h> 
#include <stdarg.h>
#include <errno.h>

sigset_t mask; 

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

void dump (char *format, ...)
{
    va_list st; 
    va_start (st, format); 
    vsyslog (LOG_INFO, format, st); 
    va_end (st); 
}

void* thr_fn (void* arg)
{
    int err, signo; 
    for (;;) {
        err = sigwait (&mask, &signo); 
        if (err != 0) { 
            syslog (LOG_ERR, "sigwait failed"); 
            exit (1); 
        }

        switch (signo) { 
            case SIGHUP:
                syslog (LOG_INFO, "Re-reading configuration file"); 
                reread (); 
                break; 
            case SIGTERM:
                syslog (LOG_INFO, "got SIGTERM, exiting"); 
                //exit (0); 
                pthread_exit (0); 
            default:
                syslog (LOG_INFO, "unexpected signal %d\n", signo); 
                break; 
        }
    }

    syslog (LOG_INFO, "%ld exit", pthread_self ()); 
    return 0; 
}

int main (int argc, char *argv[])
{
    printids ("before daemonize "); 
    daemonize ("haihai"); 
    printids ("after daemonize  "); 
    if(already_running ())
        return 1; 

    int err; 
    pthread_t tid; 
#if 0
    struct sigaction sa, old_sa; 
#  if 1
    sa.sa_handler = SIG_DFL; 
#  else
    sa.sa_handler = SIG_IGN; 
#  endif
    sigemptyset (&sa.sa_mask); 
    sa.sa_flags = 0; 
    if (sigaction (SIGHUP, &sa, &old_sa) < 0)
        err_quit ("%s: can't restore SIGHUP default"); 

    syslog (LOG_INFO, "old handler for SIGHUP is %d", old_sa.sa_handler); 
#endif

    sigfillset (&mask); 
    // default action is ignore !
    //sigdelset (&mask, SIGHUP); 
    if ((err = pthread_sigmask (SIG_BLOCK, &mask, NULL)) != 0)
        err_exit (err, "pthread_sigmask SIG_BLOCK error"); 

    err = pthread_create (&tid, NULL, thr_fn, 0); 
    if (err != 0)
        err_exit (err, "can't create thread"); 

    sleep (200); 
    syslog (LOG_INFO, "wakeup and start join thread"); 
    pthread_kill (tid, SIGTERM); 
    pthread_join (tid, NULL); 
    syslog (LOG_INFO, "join signal thread"); 
    closelog (); 
    return 0; 
}
