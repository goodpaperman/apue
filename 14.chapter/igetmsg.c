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
	char ctlbuf[BUFFSIZE+1], datbuf[BUFFSIZE+1]; 
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
			fprintf (stderr, "getmsg error %d, %s\n", errno, strerror(errno)); 
			exit(-1); 
		}

		fprintf (stderr, "[%08x] flag = %d, ctl.len = %d, dat.len = %d, ret = %d\n", getpid (), flag, ctl.len, dat.len, n); 

		if (ctl.len <= 0 && dat.len <= 0)
		{
			fprintf (stderr, "get an empty message, exit..\n"); 
			exit (0); 
		}
		else if (ctl.len > 0) { 
			fprintf (stderr, "get an control message\n"); 
			if (ctl.buf[ctl.len-1] != '\n') 
				ctl.buf[ctl.len++] = '\n'; 
			
			if (write(STDOUT_FILENO, ctl.buf, ctl.len) != ctl.len) { 
				fprintf (stderr, "write error %d, %s\n", errno, strerror(errno));
				exit (-1); 
			}
		}

		if (dat.len > 0) {
			if (ctl.len <= 0)
				fprintf (stderr, "get an data message\n"); 

			if (dat.buf[dat.len-1] != '\n')
				dat.buf[dat.len++] = '\n'; 

			// on pipe broken, write will exit (1) directly ? don't known why
			//fprintf (stderr, "prepare to write %d\n", dat.len); 
			if (write (STDOUT_FILENO, dat.buf, dat.len) != dat.len){ 
				fprintf (stderr, "write error %d, %s\n", errno, strerror(errno)); 
				exit (-1); 
			}

			//fprintf (stderr, "write %d\n", dat.len); 
		}
	}
	return 0; 
}
