//#include "../apue.h" 
#include <signal.h> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 128

int s_pipe (int fd[2])
{
	// on solaris
	return (pipe(fd)); 
}

static void sig_pipe (int); 

int main (int argc, char *argv[])
{
    if (argc < 2) { 
        printf ("usage: padd2 <add2-program>\n"); 
        return 0; 
    }

    int n;
    if (signal (SIGPIPE, sig_pipe) == SIG_ERR) {
        printf ("signal error\n"); 
		return 0; 
	}

	int fd[2]; 
    if (s_pipe (fd) < 0) {
        printf ("pipe error\n"); 
		return 0; 
	}

    char line[MAXLINE]; 
    pid_t pid = fork (); 
    if (pid < 0) {
        printf ("fork error\n"); 
		return 0; 
	}
    else if (pid > 0)
    {
        close (fd[1]); 
        while (fgets (line, MAXLINE, stdin) != NULL) { 
            n = strlen (line); 
            if (write (fd[0], line, n) != n){
                printf ("write error to pipe\n"); 
				return 0; 
			}
            if ((n = read (fd[0], line, MAXLINE)) < 0) {
                printf ("read error from pipe\n"); 
				return 0; 
			}

            if (n == 0) { 
                printf ("child closed pipe\n"); 
                break;
            }
            line[n] = 0; 
            if (fputs (line, stdout) == EOF) {
                printf ("fputs error\n"); 
				return 0; 
			}
        }

        if (ferror (stdin)) {
            printf ("fputs error\n"); 
			return 0; 
		}

        return 0; 
    }
    else { 
        close (fd[0]); 
        if (fd[1] != STDIN_FILENO) { 
            if (dup2 (fd[1], STDIN_FILENO) != STDIN_FILENO) {
                printf ("dup2 error to stdin\n"); 
				return 0; 
			}
            close (fd[0]); 
        }

        if (fd[1] != STDOUT_FILENO) { 
            if (dup2 (fd[1], STDOUT_FILENO) != STDOUT_FILENO) {
                printf ("dup2 error to stdout\n"); 
				return 0; 
			}
            close (fd[1]); 
        }

        if (execl (argv[1], argv[2], (char *)0) < 0) {
            printf ("execl error\n"); 
			return 0; 
		}
    }

    return 0; 
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
