#include <stdio.h> 
#include <stropts.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>

#define BUFFSIZE 4096

void set_wr_mode (int fd)
{
	int mode = 0; 
	ioctl (fd, I_GWROPT, &mode); 
	fprintf (stderr, "old write mode: %d\n", mode); 

#if 0
	// not work, use 0 has the same effect.
	mode = SNDZERO | SNDPIPE;
#else 
	mode = 0; 
#endif 

	ioctl (fd, I_SWROPT, mode); 
	fprintf (stderr, "new write mode: %d\n", mode); 
}

int main (void)
{
	int n, flag; 
	char ctlbuf[BUFFSIZE], datbuf[BUFFSIZE]; 
	struct strbuf ctl, dat; 
	ctl.buf = ctlbuf; 
	ctl.maxlen = BUFFSIZE; 
	dat.buf = datbuf; 
	dat.maxlen = BUFFSIZE; 
	set_wr_mode (STDOUT_FILENO); 
	for (;;) { 
		flag = 0; 
		if ((n = getmsg(STDIN_FILENO, &ctl, &dat, &flag)) < 0) {
			fprintf (stderr, "getmsg error %d, %s\n", errno, strerror(errno)); 
			exit(-1); 
		}

		fprintf (stderr, "[%08x] flag = %d, ctl.len = %d, dat.len = %d\n", getpid (), flag, ctl.len, dat.len); 

		if (dat.len == 0)
			exit (0); 
		else if (dat.len > 0) {
			//ctl.len = -1; 
			//flag = 0;

			// to test zero length send
			if (dat.len == 1 && dat.buf[0] == '\n')
				dat.len = 0; 
#if 0
			dat.len = -1; 
#elif 0
			dat.len = 0; 
#elif 0
			// warning: telnet reset
			strcpy (ctl.buf, "hello world!"); 
			ctl.len = strlen(ctl.buf); 
#elif 0
			// error: EINVAL
			flag = RS_HIPRI; 
#elif 0
			// blocked, use Ctrl+C to exit
			// if use Ctrl+D you will get telnet reset
			strcpy (ctl.buf, "hello world!"); 
			ctl.len = strlen(ctl.buf); 
			flag = RS_HIPRI; 
#elif 0
			// blocked, use Ctrl+C to exit
			// if use Ctrl+D you will get telnet reset
			strcpy (ctl.buf, "hello world!"); 
			ctl.len = strlen(ctl.buf); 
			dat.len = -1; 
			flag = RS_HIPRI; 
#endif 
			if (putmsg (STDOUT_FILENO, &ctl, &dat, flag) < 0){ 
				fprintf (stderr, "putmsg error %d, %s\n", errno, strerror(errno)); 
				exit (-1); 
			}

			fprintf (stderr, "[%08x] flag = %d, ctl.len = %d, dat.len = %d\n", getpid (), flag, ctl.len, dat.len); 
		}
	}

	return 0; 
}
