#include <unistd.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h>
#include <stdlib.h>
#include "spipe_fd.h"

#define MAXLINE 128

int get_temp_fd ()
{
    char fname[128] = "/tmp/inXXXXXX";
    int fd = mkstemp (fname);
	unlink (fname); // remove temp file after close fd
    fprintf (stderr, "create temp file %s with fd %d\n", fname, fd);
    return fd;
}

int main (void)
{
    int ret, fdin, fdout, n, int1, int2; 
    char line[MAXLINE]; 
	//fdin = STDIN_FILENO; 
	//fdout = STDOUT_FILENO; 

	while (1) {
	fdin = recv_fd (STDIN_FILENO, write); 
	if (fdin < 0) {
		fprintf (stderr, "recv_fd failed, error %d\n", fdin); 
		return -1; 
	}

	fprintf (stderr, "recv fd %d, position %u\n", fdin, lseek(fdin, 0, SEEK_CUR)); 
	fdout = get_temp_fd (); 
	if (fdout < 0) { 
		fprintf (stderr, "get temp fd failed\n"); 
		return -1; 
	}

    n = read (fdin, line, MAXLINE);
	if (n > 0) {
        line[n] = 0; 
		fprintf (stderr, "source: %s\n", line); 
        if (sscanf (line, "%d%d", &int1, &int2) == 2) { 
            sprintf (line, "%d\n", int1 + int2); 
            n = strlen (line); 
            if (write (fdout, line, n) != n) {
                fprintf (stderr, "write error\n"); 
				return 0; 
			}
        }
        else { 
            if (write (fdout, "invalid args\n", 13) != 13) {
                fprintf (stderr, "write msg error\n"); 
				return 0; 
			}
        }

#if 1
		if (lseek (fdout, 0, SEEK_SET) < 0)
			fprintf (stderr, "seek to begin failed\n"); 
		else 
			fprintf (stderr, "seek to head\n"); 
#endif

		ret = send_fd (STDOUT_FILENO, fdout); 
		if (ret < 0) {
			fprintf (stderr, "send fd to peer failed, error %d\n", ret); 
			return -1; 
		}

		// fdout will be automatically closed by send_fd
		fprintf (stderr, "send fd %d\n", fdout); 
    }
	else
		fprintf (stderr, "no more data\n"); 

	close (fdin); 
	}

    return 0; 
}
