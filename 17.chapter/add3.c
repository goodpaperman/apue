
#include <signal.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>

#define MAXLINE 128

static void sig_pipe (int);

int s_pipe (int fd[2])
{
	return (pipe(fd)); 
}

int main (void)
{
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

	int n = 0; 
    char line[MAXLINE]; 
    close (fd[1]);
    while ((n = read (fd[0], line, MAXLINE)) > 0) { 
        line[n] = 0; 
        if (sscanf (line, "%d%d", &int1, &int2) == 2) { 
            sprintf (line, "%d\n", int1 + int2); 
            n = strlen (line); 
            if (write (fd[0], line, n) != n) {
                printf ("write error\n"); 
				return 0; 
			}
			printf ("i am working on %s\n", line); 
        }
        else { 
            if (write (fd[0], "invalid args\n", 13) != 13) {
                printf ("write msg error\n"); 
				return 0; 
			}
        }
    }

	if (fdetach ("./pipe") < 0) {
		printf ("fdetach error\n"); 
		return 0; 
	}
 
	printf ("detach from file pipe ok\n"); 
	close (fd[0]); 
    return 0; 
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
