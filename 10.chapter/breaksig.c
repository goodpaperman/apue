#include "../apue.h" 
#include <strings.h> 
#include <signal.h>
#include <setjmp.h> 

// 0:no jmp; 1:simple jmp; 2: sigjmp
#define USE_JMP 2
// 0:common signal; 2:apue signal; 3:sigaction
#define USE_SIGNAL 2

#if USE_JMP>0
static jmp_buf env_alrm; 
#endif 

static void sigalrm (int signo)
{
    printf ("mask in sigalrm:\n"); 
    pr_procset (); 

    // do nothing, just break read call !
#if USE_JMP==1
    longjmp (env_alrm, 1); 
#elif USE_JMP==2
    siglongjmp (env_alrm, 1); 
#endif
}

int main (int argc, char *argv[])
{
    int n; 
    char line [MAXLINE]; 
#if USE_SIGNAL==0
    if (signal (SIGALRM, sigalrm) == SIG_ERR)
        err_sys ("signal (SIGALRM) error"); 
#elif USE_SIGNAL==1 
    if (apue_signal (SIGALRM, sigalrm) == SIG_ERR)
        err_sys ("apue_signal (SIGALRM) error"); 
#else
    struct sigaction sa; 
    sa.sa_flags = SA_RESTART; 
    sigemptyset (&sa.sa_mask); 
    sa.sa_handler = sigalrm; 
    sigaction (SIGALRM, &sa, NULL); 
#endif

#if USE_JMP==1
    if (setjmp(env_alrm) != 0)
#elif USE_JMP==2
    if (sigsetjmp(env_alrm, 1) != 0)
#endif
    {
        printf ("mask after jmp:\n"); 
        pr_procset (); 
        err_quit ("read timeout"); 
    }

    do 
    {
        alarm (10); 
        printf ("mask before read:\n"); 
        pr_procset (); 
        if ((n = read (STDIN_FILENO, line, MAXLINE)) < 0)
            err_sys ("read error"); 

        printf ("mask after read:\n"); 
        pr_procset (); 
        if (strcasecmp (line, "exit") == 0)
            break; 

        alarm (0); 
        write (STDOUT_FILENO, line, n); 
    } while (1); 
    exit (0); 
}
