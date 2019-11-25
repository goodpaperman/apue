//#include "../apue.h" 
#include <signal.h> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 128

int s_pipe (int fd[2])
{
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

	if (fattach (fd[1], "./pipe") < 0) {
		printf ("fattach error\n"); 
		return 0; 
	}

	printf ("attach to file pipe ok\n"); 

    char line[MAXLINE]; 
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

	if (fdetach ("./pipe") < 0) {
		printf ("fdetach error\n"); 
		return 0; 
	}

	printf ("detach from file pipe ok\n"); 
    return 0; 
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
