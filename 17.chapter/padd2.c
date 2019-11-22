//#include "../apue.h" 
#include <signal.h> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//#define USE_FILE 1
#define MAXLINE 128
#define USE_FSPIPE

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

#ifdef USE_FSPIPE
	if (fattach (fd[1], "./pipe") < 0) {
		printf ("fattach error\n"); 
		return 0; 
	}

	printf ("attach to file pipe ok\n"); 
#endif

    char line[MAXLINE]; 
#ifndef USE_FSPIPE
    pid_t pid = fork (); 
    if (pid < 0) {
        printf ("fork error\n"); 
		return 0; 
	}
    else if (pid > 0)
    {
#endif
        close (fd[1]); 
#ifdef USE_FILE
        FILE* fp1 = fdopen (fd[0], "w"); 
        FILE* fp2 = fdopen (fd[0], "r"); 
        if (fp1 == NULL || fp2 == NULL) {
            printf ("open filep on fd failed\n"); 
			return 0; 
		}
#endif 

        while (fgets (line, MAXLINE, stdin) != NULL) { 
            n = strlen (line); 
#ifdef USE_FILE
            if (fwrite (line, 1, n, fp1) != n) {
                printf ("fwrite error to pipe"); 
				return 0; 
			}

            fflush (fp1); 
            printf ("waiting reply\n"); 
            if (fgets (line, MAXLINE, fp2) == NULL) {
                printf ("fread error from pipe"); 
				return 0; 
			}
#else
            if (write (fd[0], line, n) != n){
                printf ("write error to pipe\n"); 
				return 0; 
			}
            if ((n = read (fd[0], line, MAXLINE)) < 0) {
                printf ("read error from pipe\n"); 
				return 0; 
			}
#endif

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

#ifdef USE_FSPIPE
		if (fdetach ("./pipe") < 0) {
			printf ("fdetach error\n"); 
			return 0; 
		}

		printf ("detach from file pipe ok\n"); 
#endif

        return 0; 
#ifndef USE_FSPIPE
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

#  if 0
        // not work after exec
        if (setvbuf (stdin, NULL, _IOLBF, 0) != 0)
            err_sys ("setvbuf error"); 
        if (setvbuf (stdout, NULL, _IOLBF, 0) != 0)
            err_sys ("setvbuf error"); 
#  endif 
        if (execl (argv[1], argv[2], (char *)0) < 0) {
            printf ("execl error\n"); 
			return 0; 
		}
    }

    return 0; 
#endif
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
