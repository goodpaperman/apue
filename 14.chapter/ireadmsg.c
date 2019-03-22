#include <stdio.h> 
#include <stropts.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>

#if 1
#  define BUFFSIZE 4096
#else
#  define BUFFSIZE 4
#endif

void set_rd_mode (int fd)
{
	int mode = 0, proto = 0; 
	ioctl (fd, I_GRDOPT, &mode); 
	fprintf (stderr, "current mode %d\n", mode); 

#if 0
	mode = RMSGN; 
#elif 0
	mode = RMSGD; 
#else 
	mode = RNORM; 
#endif 

#if 0
	proto = RPROTDAT; 
#elif 0
	proto = RPROTDIS; 
#else 
	proto = RPROTNORM; 
#endif 

	ioctl (fd, I_SRDOPT, mode | proto); 
	fprintf (stderr, "set mode %d\n", mode | proto); 

	ioctl (fd, I_GRDOPT, &mode); 
	fprintf (stderr, "now mode %d\n", mode); 
}

int main (void)
{
	int n; 
	char buf[BUFFSIZE+1];
	sleep (1); // wait iputmsg to output mode
	set_rd_mode (STDIN_FILENO); 
	for (;;) { 
#if 1
		sleep (10); 
#endif 
		if ((n = read(STDIN_FILENO, buf, BUFFSIZE)) < 0) {
			fprintf (stderr, "read error %d, %s\n", errno, strerror(errno)); 
			// if (errno == EBADMSG)
			exit(-1); 
		}

		fprintf (stderr, "[%08x] ret = %d\n", getpid (), n); 

		if (n == 0)
		{
			fprintf (stderr, "get an empty message, exit..\n"); 
			exit (0); 
		}
		else {
			fprintf (stderr, "get an message\n"); 
			if (buf[n-1] != '\n') 
				buf[n++] = '\n'; 
			
			if (write(STDOUT_FILENO, buf, n) != n) { 
				fprintf (stderr, "write error %d, %s\n", errno, strerror(errno));
				exit (-1); 
			}
		}
	}

	return 0; 
}
