#include <stdio.h> 
#include <poll.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stropts.h>
#include <stdlib.h> 
#include <string.h> 

#define POLL_STDOUT

#ifdef POLL_STDOUT
#  define PFD_MAX 2 
#else 
#  define PFD_MAX 1
#endif 

#define BUFFSIZE 1024

void readmsg (int fd)
{
	int flag = 0, n = 0; 
	char ctlbuf[BUFFSIZE+1], datbuf[BUFFSIZE+1];
    struct strbuf ctl, dat;
    ctl.buf = ctlbuf;
    ctl.maxlen = BUFFSIZE;
    dat.buf = datbuf;
    dat.maxlen = BUFFSIZE;
	if ((n = getmsg(fd, &ctl, &dat, &flag)) < 0) {
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

        if (write(STDERR_FILENO, ctl.buf, ctl.len) != ctl.len) {
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
        if (write (STDERR_FILENO, dat.buf, dat.len) != dat.len){
            fprintf (stderr, "write error %d, %s\n", errno, strerror(errno)); 
            exit (-1);
        }

        //fprintf (stderr, "write %d\n", dat.len);
    }
}

void readpmsg (int fd)
{
	int flag = MSG_BAND, n = 0, band = 0; 
	char ctlbuf[BUFFSIZE+1], datbuf[BUFFSIZE+1];
    struct strbuf ctl, dat;
    ctl.buf = ctlbuf;
    ctl.maxlen = BUFFSIZE;
    dat.buf = datbuf;
    dat.maxlen = BUFFSIZE;
	if ((n = getpmsg(fd, &ctl, &dat, &band, &flag)) < 0) {
        fprintf (stderr, "getpmsg error %d, %s\n", errno, strerror(errno)); 
        exit(-1);
    }

    fprintf (stderr, "[%08x] flag = %d, band = %d, ctl.len = %d, dat.len = %d, ret = %d\n", getpid (), flag, band, ctl.len, dat.len, n); 

    if (ctl.len <= 0 && dat.len <= 0)
    {
        fprintf (stderr, "get an empty message, exit..\n");
        exit (0);
    }
    else if (ctl.len > 0) {
        fprintf (stderr, "get an control message\n");
        if (ctl.buf[ctl.len-1] != '\n')
        ctl.buf[ctl.len++] = '\n';

        if (write(STDERR_FILENO, ctl.buf, ctl.len) != ctl.len) {
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
        if (write (STDERR_FILENO, dat.buf, dat.len) != dat.len){
            fprintf (stderr, "write error %d, %s\n", errno, strerror(errno)); 
            exit (-1);
        }

        //fprintf (stderr, "write %d\n", dat.len);
    }
}

void writemsg (int fd)
{
	int flag = 0, n = 0; 
	char ctlbuf[BUFFSIZE+1], datbuf[BUFFSIZE+1];
    struct strbuf ctl, dat;
    ctl.buf = ctlbuf;
    ctl.maxlen = BUFFSIZE;
    dat.buf = datbuf;
    dat.maxlen = BUFFSIZE;

	strcpy (dat.buf, "#####"); 
	dat.len = strlen (dat.buf)+1; 

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
#elif 0
    if (dat.len % 2 == 0) {
        // add some random
        strcpy (ctl.buf, dat.buf);
        ctl.len = dat.len;
    }
#endif
	if ((n = putmsg(fd, &ctl, &dat, flag)) < 0) {
        fprintf (stderr, "putmsg error %d, %s\n", errno, strerror(errno)); 
        exit(-1);
    }

    fprintf (stderr, "[%08x] flag = %d, ctl.len = %d, dat.len = %d, ret = %d\n", getpid (), flag, ctl.len, dat.len, n); 
}

void writepmsg (int fd)
{
	int flag = 0, n = 0, band = 0; 
	char ctlbuf[BUFFSIZE+1], datbuf[BUFFSIZE+1];
    struct strbuf ctl, dat;
    ctl.buf = ctlbuf;
    ctl.maxlen = BUFFSIZE;
    dat.buf = datbuf;
    dat.maxlen = BUFFSIZE;

	strcpy (dat.buf, "#####"); 
	dat.len = strlen (dat.buf)+1; 

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
#elif 0
    if (dat.len % 2 == 0) {
        // add some random
        strcpy (ctl.buf, dat.buf);
        ctl.len = dat.len;
    }
#elif 0
    band = 128;
    flag = MSG_BAND;
#elif 0
    strcpy (ctl.buf, "hello world!");
    ctl.len = strlen(ctl.buf);
    band = 127;
    flag = MSG_BAND;
#elif 0
    strcpy (ctl.buf, "hello world!");
    ctl.len = strlen(ctl.buf);
    dat.len = -1;
    band = 127;
    flag = MSG_BAND;
#elif 1
    if (dat.len % 2 == 0) {
        // add some random
        strcpy (ctl.buf, dat.buf);
    	ctl.len = dat.len;
    }
    band = 127;
    flag = MSG_BAND;
#endif

	if ((n = putpmsg(fd, &ctl, &dat, band, flag)) < 0) {
        fprintf (stderr, "putpmsg error %d, %s\n", errno, strerror(errno)); 
        exit(-1);
    }

    fprintf (stderr, "[%08x] flag = %d, band = %d, ctl.len = %d, dat.len = %d, ret = %d\n", getpid (), flag, band, ctl.len, dat.len, n); 
}

int main (int argc, char *argv[])
{
	int ret = 0, has_error = 0; 
	struct pollfd pfd[PFD_MAX] = { 0 }; 
	pfd[0].fd = STDIN_FILENO; 
	pfd[0].events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI; 
#ifdef POLL_STDOUT
	pfd[1].fd = STDOUT_FILENO; 
	pfd[1].events = POLLOUT /*| POLLWRBAND*/ ; 
#endif 

	while (!has_error) 
	{
		fprintf (stderr, "poll %d handles\n", PFD_MAX); 
		ret = poll (pfd, PFD_MAX, -1); 
		if (ret == -1)
		{
			if (errno == EINTR){ 
				fprintf (stderr, "interrupt by signal\n"); 
				continue; 
			}
			
			fprintf (stderr, "poll error %d\n", errno); 
			break; 
		}
		else if (ret == 0){ 
			fprintf (stderr, "poll over but no event, errno %d\n", errno); 
			continue; 
		}
		
		for (int i=0; i<PFD_MAX; ++ i) { 
			fprintf (stderr, "revents 0x%x\n", pfd[i].revents); 
			// handle any type of read/write, other type is invalid after read/write messages.
			if (pfd[i].revents & POLLPRI) { 
				fprintf (stderr, "poll %d priority\n", pfd[i].fd); 
				readpmsg (pfd[i].fd);
			}
			else if (pfd[i].revents & POLLRDBAND) { 
				fprintf (stderr, "poll %d read band\n", pfd[i].fd); 
				readpmsg (pfd[i].fd);
			}
			else if (pfd[i].revents & POLLRDNORM) { 
				fprintf (stderr, "poll %d read normal\n", pfd[i].fd); 
				readmsg (pfd[i].fd);
			}
			else if (pfd[i].revents & POLLIN) { 
				fprintf (stderr, "poll %d in\n", pfd[i].fd); 
				readmsg (pfd[i].fd); 
			}

			if (pfd[i].revents & POLLWRBAND) { 
				fprintf (stderr, "poll %d write band\n", pfd[i].fd); 
				writepmsg (pfd[i].fd); 
			}
			else if (pfd[i].revents & POLLWRNORM) { 
				fprintf (stderr, "poll %d write normal\n", pfd[i].fd); 
				writemsg (pfd[i].fd); 
			}
			else if (pfd[i].revents & POLLOUT) { 
				fprintf (stderr, "poll %d out\n", pfd[i].fd); 
				writemsg (pfd[i].fd); 
			}
			else if (pfd[i].revents & POLLERR) { 
				has_error = 1; 
				fprintf (stderr, "poll %d error\n", pfd[i].fd); 
				break; 
			}

			if (pfd[i].revents & POLLHUP) { 
				has_error = 1; 
				fprintf (stderr, "poll %d hangup\n", pfd[i].fd); 
				break; 
			}

			if (pfd[i].revents & POLLNVAL) { 
				has_error = 1; 
				fprintf (stderr, "poll %d invalid\n", pfd[i].fd); 
				break; 
			}
		}
	}
	return 0; 
}
