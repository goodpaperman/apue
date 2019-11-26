#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>

#define MAXLINE 128

static void sig_pipe (int);

#define FIFO_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int serv_listen (const char *name)
{
	int tempfd; 
	int fd[2];
	unlink (name); 
	tempfd = creat (name, FIFO_MODE); 
	if (tempfd < 0) {
		printf ("creat failed\n"); 
		return -1; 
	}

	if (close (tempfd) < 0) {
		printf ("close temp fd failed\n"); 
		return -2; 
	}

    if (pipe (fd) < 0) {
        printf ("pipe error\n"); 
		return -3; 
	}

	if (ioctl (fd[1], I_PUSH, "connld") < 0) { 
		printf ("I_PUSH connld failed\n"); 
		close (fd[0]); 
		close (fd[1]); 
		return -4; 
	}

	printf ("push connld ok\n"); 
	if (fattach (fd[1], name) < 0) {
		printf ("fattach error\n"); 
		close (fd[0]); 
		close (fd[1]); 
		return -5; 
	}

	printf ("attach to file pipe ok\n"); 
	close (fd[1]); 
	return fd[0];
}

int serv_accept (int listenfd, uid_t *uidptr)
{
	struct strrecvfd recvfd; 
	if (ioctl (listenfd, I_RECVFD, &recvfd) < 0) { 
		printf ("I_RECVFD from listen fd failed\n"); 
		return -1; 
	}

	if (uidptr)
		*uidptr = recvfd.uid; 

	return recvfd.fd; 
}

int main (void)
{
    if (signal (SIGPIPE, sig_pipe) == SIG_ERR) {
        printf ("signal error\n"); 
		return 0; 
	}
 
	int listenfd = serv_listen ("./pipe"); 
	if (listenfd < 0)
		return 0; 

	int acceptfd = 0; 
	int n = 0, int1 = 0, int2 = 0; 
	char line[MAXLINE]; 
	while ((acceptfd = serv_accept (listenfd, &uid)) >= 0)
	{
    	while ((n = read (acceptfd, line, MAXLINE)) > 0) { 
    	    line[n] = 0; 
			printf ("source: %s\n", line); 
    	    if (sscanf (line, "%d%d", &int1, &int2) == 2) { 
    	        sprintf (line, "%d\n", int1 + int2); 
    	        n = strlen (line); 
    	        if (write (acceptfd, line, n) != n) {
    	            printf ("write error\n"); 
					return 0; 
				}
				printf ("i am working on %d + %d = %s\n", int1, int2, line); 
    	    }
    	    else { 
    	        if (write (acceptfd, "invalid args\n", 13) != 13) {
    	            printf ("write msg error\n"); 
					return 0; 
				}
    	    }
    	}

		close (acceptfd); 
	}

	if (fdetach ("./pipe") < 0) {
		printf ("fdetach error\n"); 
		return 0; 
	}
 
	printf ("detach from file pipe ok\n"); 
	close (listenfd); 
    return 0; 
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
