#include "../apue.h" 
#include <unistd.h> 
#include <sys/wait.h> 

#define PAGER "${PAGER:-more}"
#if 1
#  define POPEN popen
#  define PCLOSE pclose
#else 
#  define POPEN apue_popen
#  define PCLOSE apue_pclose
#endif 

#define USE_SIG 1
#define USE_SIGACT 
#define AUTO_WAIT
//#define AUTO_WAIT_DFL

#ifdef USE_SIGACT
static void sig_cld (int signo, siginfo_t *info, void* param)
{
    int status = 0; 
    if (signo == SIGCHLD)
    {
        if (info->si_code == CLD_EXITED ||
                info->si_code == CLD_KILLED || 
                info->si_code == CLD_DUMPED)
        {
#ifdef AUTO_WAIT
            printf ("pid (auto wait in signal) = %d\n", info->si_pid); 
#else 
            if (waitpid (info->si_pid, &status, 0) < 0)
                perror ("wait(in signal) error"); 
            printf ("pid (wait in signal) = %d\n", info->si_pid); 
#endif
        }
        else 
        {
            printf ("unknown signal code %d\n", info->si_code); 
        }
    }
}
#else
static void sig_cld (int signo)
{
    pid_t pid = 0; 
    int status = 0; 
    printf ("SIGCLD received\n"); 
    if (signal (SIGCLD, sig_cld) == SIG_ERR)
        perror ("signal error"); 

    if ((pid = wait (&status)) < 0)
        perror ("wait(in signal) error"); 

    printf ("pid (wait in signal) = %d\n", pid); 
}
#endif 

void install_handler (__sighandler_t h)
{
#ifdef USE_SIGACT
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
#  ifdef AUTO_WAIT_DFL
    act.sa_sigaction = SIG_DFL; 
#  else 
    act.sa_sigaction = h; 
#  endif
    act.sa_flags = SA_SIGINFO | SA_NOCLDSTOP; 
#  ifdef AUTO_WAIT
    act.sa_flags |= SA_NOCLDWAIT; 
#  endif
    int ret = sigaction (SIGCHLD, &act, 0); 
    if (ret == -1)
        perror ("sigaction error"); 
#else    
    __sighandler_t ret = signal (SIGCLD, h);
    if (ret == SIG_ERR)
        perror ("signal error"); 
    else 
        printf ("old handler %x\n", ret); 
#endif
}

int main (int argc, char *argv[])
{
    int n = 0; 
#if USE_SIG == 1
    install_handler (sig_cld); 
#elif USE_SIG == 2
    install_handler (SIG_IGN); 
#endif

    char line[MAXLINE] = { 0 }; 
    FILE *fpin = NULL, *fpout = NULL; 
    if (argc != 2)
        err_quit ("usage: ppage <pathname>"); 

    fpin = fopen (argv[1], "r"); 
    if (fpin == NULL)
        err_sys ("can't open %s", argv[1]); 

    fpout = POPEN (PAGER, "w"); 
    if (fpout == NULL)
        err_sys ("popen %s error", PAGER); 

    while (fgets (line, MAXLINE, fpin) != NULL) { 
        if (fputs (line, fpout) == EOF)
            err_sys ("fputs error to pipe"); 
    }

    if (ferror (fpin))
        err_sys ("fgets error"); 

#if 0
    // to see pipe at parent
    sleep (60); 
#endif 

    int ret = PCLOSE (fpout); 
    if (ret == -1)
        err_sys ("pclose error"); 
    else 
        printf ("worker return %d\n", ret); 

    return 0; 
}
