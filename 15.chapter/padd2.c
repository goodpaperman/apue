#include "../apue.h" 

static void sig_pipe (int); 

int main (int argc, char *argv[])
{
    if (argc < 2) { 
        printf ("usage: padd2 <add2-program>\n"); 
        return 0; 
    }

    int n, fd1[2], fd2[2]; 
    if (signal (SIGPIPE, sig_pipe) == SIG_ERR)
        err_sys ("signal error"); 

    if (pipe (fd1) < 0 || pipe(fd2) < 0)
        err_sys ("pipe error"); 

    char line[MAXLINE]; 
    pid_t pid = fork (); 
    if (pid < 0) 
        err_sys ("fork error"); 
    else if (pid > 0)
    {
        close (fd1[0]);  // write on pipe1 as stdin for co-process
        close (fd2[1]);  // read on pipe2 as stdout for co-process
        while (fgets (line, MAXLINE, stdin) != NULL) { 
            n = strlen (line); 
            if (write (fd1[1], line, n) != n)
                err_sys ("write error to pipe"); 
            if ((n = read (fd2[0], line, MAXLINE)) < 0)
                err_sys ("read error from pipe"); 
            if (n == 0) { 
                err_msg ("child closed pipe"); 
                break;
            }
            line[n] = 0; 
            if (fputs (line, stdout) == EOF)
                err_sys ("fputs error"); 
        }

        if (ferror (stdin))
            err_sys ("fputs error"); 

        return 0; 
    }
    else { 
        close (fd1[1]); 
        close (fd2[0]); 
        if (fd1[0] != STDIN_FILENO) { 
            if (dup2 (fd1[0], STDIN_FILENO) != STDIN_FILENO)
                err_sys ("dup2 error to stdin"); 
            close (fd1[0]); 
        }

        if (fd2[1] != STDOUT_FILENO) { 
            if (dup2 (fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
                err_sys ("dup2 error to stdout"); 
            close (fd2[1]); 
        }

        if (execl (argv[1], "add2", (char *)0) < 0)
            err_sys ("execl error"); 
    }

    return 0; 
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
