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

int main (void)
{
	int n, flag; 
	char ctlbuf[BUFFSIZE], datbuf[BUFFSIZE]; 
	struct strbuf ctl, dat; 
	ctl.buf = ctlbuf; 
	ctl.maxlen = BUFFSIZE; 
	dat.buf = datbuf; 
	dat.maxlen = BUFFSIZE; 
	for (;;) { 
		flag = 0; 
#if 0
		// dead loop !
		dat.maxlen = -1; 
#endif
		if ((n = getmsg(STDIN_FILENO, &ctl, &dat, &flag)) < 0) {
			printf ("getmsg error %d, %s\n", errno, strerror(errno)); 
			exit(-1); 
		}

		fprintf (stderr, "	flag = %d, ctl.len = %d, dat.len = %d, ret = %d\n", flag, ctl.len, dat.len, n); 

		if (dat.len == 0)
		{
			fprintf (stderr, "get an empty message, exit..\n"); 
			exit (0); 
		}
		else if (dat.len > 0) {
			if (write (STDOUT_FILENO, dat.buf, dat.len) != dat.len){ 
				printf ("write error %d, %s\n", errno, strerror(errno)); 
				exit (-1); 
			}
		}
	}
	return 0; 
}
