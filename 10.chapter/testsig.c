#include "../apue.h" 

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
    int fd[2] = { 0 }; 
    if (pipe(fd) < 0)
        err_sys ("pipe error");

    pid_t pid; 
    if ((pid = fork ()) < 0)
        err_sys ("fork error"); 
    else if (pid > 0)
    {
        close (fd[0]); 
        write (fd[1], "hello world\n", 12); 
        sleep (1); 
        write (fd[1], "hello world\n", 12); 
        sleep (1); 
        write (fd[1], "hello world\n", 12); 
    }
    else 
    {
        close (fd[1]); 
        char line [MAXLINE] = { 0 }; 
        int n = read (fd[0], line, MAXLINE);
        write (STDOUT_FILENO, line, n); 
        n = read (fd[0], line, MAXLINE); 
        write (STDOUT_FILENO, line, n); 
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
