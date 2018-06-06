#include "../apue.h" 
#include <errno.h>

void sig_eater (int signum)
{
    printf ("got signal %d\n", signum); 
}

void test_abrt ()
{
    abort (); 
}

void test_fpe ()
{
    int i = 1; 
    double j = 0.0; 
    float k = i / j; // to see SIG_FPE
    printf ("k = %.2f\n", k); 
    k = i / 0; 
    printf ("k = %.2f\n", k); 
}

//#define MAXLINE 80
void test_pipe ()
{
    int ret = 0; 
    signal (SIGPIPE, sig_eater); 
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

int main ()
{
#if 0
    test_abrt (); 
#elif 0 
    test_fpe (); 
#else 
    test_pipe (); 
#endif 
    return 0; 
}
