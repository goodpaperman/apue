#include "spipe_fd.h" 
#include <stropts.h> 
#include <string.h> 
#include <unistd.h> 
#include <stdio.h>
#include <errno.h>
//#include <sys/types.h>  // for ssize_t 

#define MAXLINE 128

int send_err (int fd, int errcode, const char *msg)
{
	int n; 
	if ((n = strlen (msg)) > 0) {
		//if (writen (fd, msg, n) != n)
		if (write (fd, msg, n) != n) {
			fprintf (stderr, "%u: write %d message failed\n", getpid (), n); 
			return -1; 
		}
	}

	if (errcode >= 0)
		errcode = -1; 

	if (send_fd (fd, errcode) < 0)
		return -1; 

	return 0; 	
}

int send_fd (int fd, int fd_to_send)
{
	char buf[2]; 
	buf[0] = 0; 
	if (fd_to_send < 0) { 
		buf[1] = -fd_to_send; 
		if (buf[1] == 0)
			buf[1] = 1; 
	}
	else 
		buf[1] = 0; 

	if (write (fd, buf, 2) != 2) {
		fprintf (stderr, "%u: send first 2 bytes failed\n", getpid ()); 
		return -1; 
	}

	fprintf (stderr, "%u: send first 2 bytes ok\n", getpid ()); 
	if (fd_to_send >= 0) {
		if (ioctl (fd, I_SENDFD, fd_to_send) < 0) {
			fprintf (stderr, "%u: send fd itself failed\n", getpid ()); 
			return -1; 
		}
		else 
			fprintf (stderr, "%u: send fd itself ok\n", getpid ()); 
	}

	return 0; 
}

/*
struct strrecvfd {
	int fd; 
	uid_t uid; 
	gid_t gid; 
	char fill[8]; 
}; 
*/

int recv_fd (int fd, ssize_t (*userfunc) (int, const void*, size_t))
{
	int newfd, nread, flag, status; 
	char *ptr; 
	char buf[MAXLINE]; 
	struct strbuf dat; 
	struct strrecvfd recvfd; 

	status = -1; 
	for (;;) { 
		dat.buf = buf; 
		dat.maxlen = MAXLINE; 
		flag = 0; 
		if (getmsg (fd, NULL, &dat, &flag) < 0) {
			fprintf (stderr, "%u: getmsg error %d\n", getpid (), errno); 
			return -1; 
		}

		nread = dat.len; 
		if (nread == 0) {
			fprintf (stderr, "%u: connection closed by server, errno %d\n", getpid (), errno); 
			return -1; 
		}

		fprintf (stderr, "%u: recv %d from peer\n", getpid (), dat.len); 
		for (ptr = buf; ptr < &buf[nread]; ) {
			if (*ptr ++ == 0) { 
				if (ptr != &buf[nread-1]) {
					fprintf (stderr, "%u: message format error\n", getpid ()); 
					return -1; 
				}

				fprintf (stderr, "%u: msg format ok\n", getpid ()); 
				status = *ptr & 0xff; 
				if (status == 0) {
					if (ioctl (fd, I_RECVFD, &recvfd) < 0) {
						fprintf (stderr, "%u: recv fd itself failed\n", getpid ()); 
						return -1; 
					}

					fprintf (stderr, "%u: recv fd itself ok, fd = %d\n", getpid (), recvfd.fd); 
					newfd = recvfd.fd; 
				}
				else {
					fprintf (stderr, "%u: recv error code %d ok\n", getpid (), -status); 
					newfd = -status; 
				}

				nread -= 2;
			}
		}

		if (nread > 0)
			if ((*userfunc) (STDERR_FILENO, buf, nread) != nread)
				return -1; 

		if (status >= 0)
			return newfd; 
	}

	return -1; 
}
