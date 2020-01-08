#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <fcntl.h>

#include "spipe.h"
#define MAXLINE 128
static void sig_pipe (int);

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
	uid_t uid = 0; 
	while ((acceptfd = serv_accept (listenfd, &uid)) >= 0)
	{
		printf ("accept a client, fd = %d, uid = %ld\n", acceptfd, uid); 
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

	close (listenfd); 
    return 0; 
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
