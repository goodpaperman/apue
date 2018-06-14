#include "../apue.h" 
#include <errno.h>

void sig_eater (int signum)
{
    printf ("got signal %d\n", signum); 
    signal(signum, sig_eater); 
}

void test_abrt ()
{
    signal (SIGABRT, SIG_IGN); 
    //signal (SIGABRT, sig_eater); 
    abort (); 
    // effect same
    //kill(0, SIGABRT); 
}

void test_alrm ()
{
    signal (SIGALRM, SIG_IGN); 
    //signal (SIGALRM, sig_eater); 
    alarm (3); 
    sleep (10); 
    printf ("i am not die\n"); 
}

void test_fpe ()
{
    int i = 1; 
    signal (SIGABRT, SIG_IGN); 
    //signal (SIGFPE, sig_eater); 
    double j = 0.0; 
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
    signal (SIGPIPE, SIG_IGN); 
    //signal (SIGPIPE, sig_eater); 
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
    //signal (signum, sig_eater); 
    signal (signum, SIG_IGN); 
    printf ("this is %d\n", getpid ()); 
    sleep (20); 
    printf ("I am not die\n"); 
}

void test_segv ()
{
    int *p = 0; 
    signal (SIGSEGV, SIG_IGN); 
    //signal (SIGSEGV, sig_eater); 
    printf ("this is %d\n", *p); 
    printf ("pointer 0x%x\n", p); 
}

void test_ttin ()
{
    // start this process in background
    signal (SIGTTIN, SIG_IGN); 
    //signal (SIGTTIN, sig_eater); 
    sleep (3); 

    char c = 0; 
    int ret = scanf ("%c", &c); 
    printf ("got %c, ret %d, error %d\n", c, ret, errno); 
}

void test_ttou ()
{
    // start this process in background
    signal (SIGTTOU, SIG_IGN); 
    //signal (SIGTTOU, sig_eater); 
    sleep (3); 

    int ret = write (STDOUT_FILENO, "this is me\n", 12); 
    FILE* f = fopen ("dbg.txt", "w+"); 
    fprintf (f, "put ret %d, error %d\n", ret, errno); 
    fclose (f); 
}

void test_winch ()
{
    signal (SIGWINCH, SIG_IGN); 
    //signal (SIGWINCH, sig_eater); 
    sleep (5); 
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
#elif 1
    test_winch (); 
#endif
    return 0; 
}
