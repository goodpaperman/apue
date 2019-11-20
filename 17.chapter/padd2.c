#include "../apue.h" 

//#define USE_FILE 1

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
#ifdef USE_FILE
        FILE* fp1 = fdopen (fd1[1], "w"); 
        FILE* fp2 = fdopen (fd2[0], "r"); 
        if (fp1 == NULL || fp2 == NULL)
            err_sys ("open filep on fd failed"); 
#endif 

        while (fgets (line, MAXLINE, stdin) != NULL) { 
            n = strlen (line); 
#ifdef USE_FILE
            if (fwrite (line, 1, n, fp1) != n)
                err_sys ("fwrite error to pipe"); 
            fflush (fp1); 
            printf ("waiting reply\n"); 
            if (fgets (line, MAXLINE, fp2) == NULL)
                err_sys ("fread error from pipe"); 
#else
            if (write (fd1[1], line, n) != n)
                err_sys ("write error to pipe"); 
            if ((n = read (fd2[0], line, MAXLINE)) < 0)
                err_sys ("read error from pipe"); 
#endif

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

#if 0
        // not work after exec
        if (setvbuf (stdin, NULL, _IOLBF, 0) != 0)
            err_sys ("setvbuf error"); 
        if (setvbuf (stdout, NULL, _IOLBF, 0) != 0)
            err_sys ("setvbuf error"); 
#endif 
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
