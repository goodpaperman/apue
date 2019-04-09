#include <stdio.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <signal.h> 
#include "../apue.h" 

#if 1
#  define BUFFSIZE 4096
#else
#  define BUFFSIZE 4
#endif

volatile int quit = 0; 
void sigint (int signo)
{
	printf ("SIGINT caught, exiting...\n"); 
	quit = 1; 
}

void sigio (int signo)
{
	int n = 0; 
	char buf[BUFFSIZE+1]; 
	if ((n = read(STDIN_FILENO, buf, BUFFSIZE)) < 0) {
		fprintf (stderr, "read error %d, %s\n", errno, strerror(errno)); 
		exit(-1); 
	}

	fprintf (stderr, "[%08x] read len = %d\n", getpid (), n); 
	if (n > 0) {
		if (buf[n-1] != '\n')
			buf[n++] = '\n'; 

		if (write (STDERR_FILENO, buf, n) != n){ 
			fprintf (stderr, "write error %d, %s\n", errno, strerror(errno)); 
			exit (-1); 
		}

		//fprintf (stderr, "write %d\n", dat.len); 
	}
    else 
        //quit = 1; 
        exit (-1); 
}


int main (void)
{
	int ret = 0; 
	signal (SIGINT, sigint); 

	//signal (SIGIO, sigio); 
	// use sigaction to get non-once effect.
	struct sigaction act; 
	sigemptyset (&act.sa_mask); 
	act.sa_flags = 0; 
	act.sa_handler = sigio; 
	sigaction (SIGIO, &act, NULL); 

	ret = fcntl (STDIN_FILENO, F_SETOWN, getpid ()); 
	if (ret != 0) {
		fprintf (stderr, "ioctl failed, errno %d\n", errno); 
		exit (0); 
	}

    set_fl (STDIN_FILENO, O_ASYNC); 
	while (!quit) {
		sleep (1); 
	}
	return 0; 
}
