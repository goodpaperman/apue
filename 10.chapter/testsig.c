#include "../apue.h" 
#include <errno.h>
#include <sys/resource.h> 
#include <limits.h>
#include <stdio.h> 
#include <signal.h> 

#define USE_SIGACT 1

#if USE_SIGACT == 0

void sig_eater (int signum)
{
    printf ("got signal %d\n", signum); 
    signal(signum, sig_eater); 
}

#elif USE_SIGACT == 1

void sig_eater (int signum)
{
    printf ("got signal %d\n", signum); 
}

#  define signal apue_signal

#else

void sig_eater (int signum, siginfo_t *si, void *cntx)
{
    printf ("caught signal %d\n", signum); 
    printf ("siginfo: \n"
            "signo: %d\n"
            "error: %d\n"
            "code : %d\n"
            "pid  : %d\n"
            "uid  : %d\n"
            "addr : %p\n"
            "status: %d\n"
            "overrun: %d\n"
            "band : %d\n\n", 
            si->si_signo, 
            si->si_errno, 
            si->si_code, 
            si->si_pid, 
            si->si_uid, 
            si->si_addr, 
            si->si_status, 
            si->si_overrun,
            si->si_band); 

    if (signum == SIGFPE || 
            signum == SIGSEGV)
        abort (); 
}

int sigactnal (int signo, void (*sighandler) (int, siginfo_t*, void*))
{
    struct sigaction act; 
    sigemptyset (&act.sa_mask); 
    act.sa_flags = SA_SIGINFO; 
    act.sa_sigaction = sig_eater; 
    return sigaction (signo, &act, NULL); 
}

#endif

void test_abrt ()
{
#if USE_SIGACT == 2
    sigactnal (SIGABRT, sig_eater); 
#else 
    //signal (SIGABRT, SIG_IGN); 
    signal (SIGABRT, sig_eater); 
#endif

    abort (); 
    // effect same
    //kill(0, SIGABRT); 
}

void test_alrm ()
{
#if USE_SIGACT == 2
    sigactnal (SIGALRM, sig_eater); 
#else 
    //signal (SIGALRM, SIG_IGN); 
    signal (SIGALRM, sig_eater); 
#endif
    alarm (3); 
    sleep (10); 
    printf ("i am not die\n"); 
}

void test_fpe ()
{
    int i = 1; 
#if USE_SIGACT == 2
    sigactnal (SIGFPE, sig_eater); 
#else 
    //signal (SIGFPE, SIG_IGN); 
    signal (SIGFPE, sig_eater); 
#endif
    double j = 0.0; 
    printf ("address of j: %p\n", &j); 
    float k = i / j; // to see SIG_FPE
    printf ("k = %.2f\n", k); 
    k = i / 0; 
    //kill (0, SIGFPE); 
    printf ("k = %.2f\n", k); 
}

//#define MAXLINE 80
void test_pipe ()
{
    int ret = 0; 
#if USE_SIGACT == 2
    sigactnal (SIGPIPE, sig_eater); 
#else 
    //signal (SIGPIPE, SIG_IGN); 
    signal (SIGPIPE, sig_eater); 
#endif
    int fd[2] = { 0 }; 
    if (pipe(fd) < 0)
        err_sys ("pipe error");

    pid_t pid; 
    if ((pid = fork ()) < 0)
        err_sys ("fork error"); 
    else if (pid > 0)
    {
        close (fd[0]); 
        ret = write (fd[1], "hello world\n", 12); 
        //write (STDOUT_FILENO, "write1\n", 7); 
        printf ("write %d, errno %d\n", ret, errno); 
        sleep (1); 
        ret = write (fd[1], "hello world\n", 12); 
        //write (STDOUT_FILENO, "write2\n", 7); 
        printf ("write %d, errno %d\n", ret, errno); 
        sleep (1); 
        ret = write (fd[1], "hello world\n", 12); 
        //write (STDOUT_FILENO, "write3\n", 7); 
        printf ("write %d, errno %d\n", ret, errno); 
    }
    else 
    {
        close (fd[1]); 
#if 0
        // to test whether close pipe works
        close (fd[2]); 
        sleep (5); 
        return; 
#endif 
        char line [MAXLINE] = { 0 }; 
        ret = read (fd[0], line, MAXLINE);
        //write (STDOUT_FILENO, line, n); 
        printf ("read %d, errno %d\n", ret, errno); 
        ret = read (fd[0], line, MAXLINE); 
        //write (STDOUT_FILENO, line, n); 
        printf ("read %d, errno %d\n", ret, errno); 
    }
}

void test_wait (int signum)
{
#if USE_SIGACT == 2
    sigactnal (signum, sig_eater); 
#else 
    signal (signum, sig_eater); 
    //signal (signum, SIG_IGN); 
#endif
    printf ("this is %d\n", getpid ()); 
    sleep (20); 
    printf ("I am not die\n"); 
}

void test_segv ()
{
    int *p = 0; 
#if USE_SIGACT == 2
    sigactnal (SIGSEGV, sig_eater); 
#else 
    //signal (SIGSEGV, SIG_IGN); 
    signal (SIGSEGV, sig_eater); 
#endif
    printf ("this is %d\n", *p); 
    printf ("pointer 0x%x\n", p); 
}

void test_ttin ()
{
    // start this process in background
#if USE_SIGACT == 2
    sigactnal (SIGTTIN, sig_eater); 
#else 
    //signal (SIGTTIN, SIG_IGN); 
    signal (SIGTTIN, sig_eater); 
#endif
    sleep (3); 

    char c = 0; 
    int ret = scanf ("%c", &c); 
    printf ("got %c, ret %d, error %d\n", c, ret, errno); 
}

void test_ttou ()
{
    // start this process in background
#if USE_SIGACT == 2
    sigactnal (SIGTTOU, sig_eater); 
#else 
    //signal (SIGTTOU, SIG_IGN); 
    signal (SIGTTOU, sig_eater); 
#endif
    sleep (3); 

    int ret = write (STDOUT_FILENO, "this is me\n", 12); 
    FILE* f = fopen ("dbg.txt", "w+"); 
    fprintf (f, "put ret %d, error %d\n", ret, errno); 
    fclose (f); 
}

void test_winch ()
{
#if USE_SIGACT == 2
    sigactnal (SIGWINCH, sig_eater); 
#else 
    signal (SIGWINCH, SIG_IGN); 
    //signal (SIGWINCH, sig_eater); 
#endif
    sleep (5); 
}

void test_xcpu ()
{
#if USE_SIGACT == 2
    sigactnal (SIGXCPU, sig_eater); 
#else 
    //signal (SIGXCPU, SIG_IGN); 
    signal (SIGXCPU, sig_eater); 
#endif

    struct rlimit rl = { 0 }; 
    getrlimit (RLIMIT_CPU, &rl); 
    printf ("cpu limit: %d, %d\n", rl.rlim_cur, rl.rlim_max); 
    rl.rlim_cur = 1;  // cause SIGXCPU
#if 0
    // set hard limit will cause SIGKILL
    rl.rlim_max = 1; 
#endif
    setrlimit (RLIMIT_CPU, &rl); 
    getrlimit (RLIMIT_CPU, &rl); 
    printf ("cpu limit: %d, %d\n", rl.rlim_cur, rl.rlim_max); 
    for (int i=0; i<INT_MAX; ++ i)
        i = i + 1; 

    printf ("I am not die\n"); 
}

void test_xfsz ()
{
#if USE_SIGACT == 2
    sigactnal (SIGXFSZ, sig_eater); 
#else 
    //signal (SIGXFSZ, SIG_IGN); 
    signal (SIGXFSZ, sig_eater); 
#endif

    struct rlimit rl = { 0 }; 
    getrlimit (RLIMIT_FSIZE, &rl); 
    printf ("fsz limit: %d, %d\n", rl.rlim_cur, rl.rlim_max); 
    rl.rlim_cur = 1;  // cause SIGXFSZ
#if 0
    // set hard limit will cause SIGKILL
    rl.rlim_max = 1; 
#endif
    setrlimit (RLIMIT_FSIZE, &rl); 
    getrlimit (RLIMIT_FSIZE, &rl); 
    printf ("fsz limit: %d, %d\n", rl.rlim_cur, rl.rlim_max); 

    FILE* f = fopen ("tmp.txt", "w"); 
    while (f)
    {
        fputs ("hello world\n", f); 
    }

    fclose (f); 
    printf ("I am not die\n"); 
}


int main ()
{
#if 0
    test_abrt (); 
#elif 0
    test_alrm (); 
#elif 0
    test_fpe (); 
#elif 0
    test_pipe (); 
#elif 0
    test_wait (SIGQUIT); 
#elif 0
    test_wait (SIGINT); 
#elif 0
    test_wait (SIGTERM); 
#elif 0
    test_wait (SIGSTOP); 
#elif 0
    test_wait (SIGTSTP); 
#elif 0
    test_wait (SIGUSR1); 
#elif 0
    test_wait (SIGUSR2); 
#elif 0
    test_segv (); 
#elif 0
    test_ttin (); 
#elif 0
    test_ttou (); 
#elif 0
    test_winch (); 
#elif 0
    test_xcpu (); 
#elif 1
    test_xfsz (); 
#endif
    return 0; 
}
