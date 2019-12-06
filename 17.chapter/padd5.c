//#include "../apue.h"  
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <fcntl.h>
#include <errno.h> 
#include <stddef.h> 
#include <sys/socket.h> 
#include <sys/un.h> 
#if defined(__sun) || defined(sun) 
#  include <stropts.h>
#  include <sys/stat.h>
#endif

#define CLI_PATH "/var/tmp/"
#define CLI_PERM S_IRWXU
#define MAXLINE 128
 
int cli_conn(const char *name)
{
    int fd, len, err, rval; 
    struct sockaddr_un un; 

    if ((fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) { 
        printf ("create socket failed\n"); 
        return -1; 
    }

    printf ("create socket ok\n"); 
    memset (&un, 0, sizeof (un)); 
    un.sun_family = AF_UNIX; 
    sprintf (un.sun_path, "%s%05d", CLI_PATH, getpid ()); 
    len = offsetof (struct sockaddr_un, sun_path) + strlen (un.sun_path); 
    unlink (un.sun_path); 

    if (bind (fd, (struct sockaddr *)&un, len) < 0) { 
        err = errno; 
        printf ("bind failed\n"); 
        rval = -2; 
        goto errout; 
    }

    printf ("bind socket to temp file ok\n"); 
    if (chmod (un.sun_path, CLI_PERM) < 0) { 
        err = errno; 
        printf ("chmod failed\n"); 
        rval = -3; 
        goto errout; 
    }

    printf ("change temp file mode ok\n"); 
    memset (&un, 0, sizeof (un)); 
    un.sun_family = AF_UNIX; 
    strcpy (un.sun_path, name); 
    len = offsetof (struct sockaddr_un, sun_path) + strlen (name); 
    if (connect (fd, (struct sockaddr *)&un, len) < 0) {
        err = errno; 
        printf ("connect failed\n"); 
        rval = -4; 
        goto errout; 
    }

    printf ("connect to server ok\n"); 
    return fd;
errout:
    close (fd); 
    errno = err; 
    return rval;
}

int main (int argc, char *argv[])
{
    if (argc != 1 && argc != 3) { 
        printf ("usage: padd3 <int1> <int2>\n"); 
        return 0; 
    }

    int fdin, fdout, n, m; 
    char line[MAXLINE]; 
	fdin = cli_conn ("./pipe"); 
	if (fdin < 0)
		return 0; 

	printf ("open file pipe ok, fd = %d\n", fdin); 
	fdout = fdin; 

	if (argc == 1) {
    	if ((n = read (STDIN_FILENO, line, MAXLINE)) < 0) {
        	printf ("read error from stdin\n"); 
			return 0; 
		}
	}
	else {
		sprintf (line, "%s %s", argv[1], argv[2]); 
	}

    n = strlen (line); 
    if (write (fdout, line, n) != n){
        printf ("write error to pipe\n"); 
		return 0; 
	}

	strcat (line, " = "); 
	m = strlen (line); 
    if ((n = read (fdin, line + m, MAXLINE - m)) < 0) {
        printf ("read error from pipe\n"); 
		return 0; 
	}

    if (n == 0) { 
        printf ("child closed pipe\n"); 
		return 0; 
    }

    line[m+n] = 0; 
    if (fputs (line, stdout) == EOF) {
        printf ("fputs error\n"); 
		return 0; 
	}

    if (ferror (stdin)) {
        printf ("fputs error\n"); 
		return 0; 
	}

	close (fdin);
    return 0; 
}

