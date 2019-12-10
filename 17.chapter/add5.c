#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <errno.h>
#include <time.h>
#include <stddef.h> 
#include <sys/socket.h> 
#include <sys/un.h> 

#define QLEN 10
#define STALE 30
#define MAXLINE 128
// note this check is mannually, not necessary for the connection establish
#define CONN_CHECK

static void sig_pipe (int);

//#define FIFO_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int serv_listen (const char *name)
{
    int fd, len, err, rval; 
    struct sockaddr_un un; 

    if ((fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf ("socket failed\n"); 
        return -1; 
    }

    printf ("create socket ok\n"); 
    unlink (name); 
    memset (&un, 0, sizeof(un)); 
    un.sun_family = AF_UNIX; 
    strcpy (un.sun_path, name); 
    len = offsetof (struct sockaddr_un, sun_path) + strlen (name); 

    if (bind (fd, (struct sockaddr *)&un, len) < 0) {
        err = errno; 
        printf ("bind failed\n");  
        rval = -2; 
        goto errout; 
    }

    printf ("bind socket to path ok\n"); 
    if (listen (fd, QLEN) < 0) { 
        err = errno; 
        printf ("listen failed\n"); 
        rval = -3; 
        goto errout; 
    }

    printf ("start listen on socket ok\n"); 
    return fd; 
errout:
    close (fd); 
    errno = err; 
    return rval; 
}

int serv_accept (int listenfd, uid_t *uidptr)
{
    int clifd, len, err, rval; 
    time_t staletime; 
    struct sockaddr_un un; 
    struct stat statbuf; 

    len = sizeof (un); 
    if ((clifd = accept (listenfd, (struct sockaddr *)&un, &len)) < 0) { 
        printf ("accept failed\n"); 
        return -1; 
    }

    len -= offsetof (struct sockaddr_un, sun_path); 
    un.sun_path[len] = 0; 
    printf ("accept %s ok\n", un.sun_path); 

#ifdef CONN_CHECK
    if (stat (un.sun_path, &statbuf) < 0) {
        err = errno; 
        printf ("stat failed\n"); 
        rval = -2; 
        goto errout; 
    }

    printf ("stat file ok\n"); 
    if (S_ISSOCK(statbuf.st_mode) == 0) { 
        printf ("not a socket\n"); 
        rval = -3; 
        goto errout; 
    }

    printf ("yes, it is a socket\n"); 
    if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) || 
        (statbuf.st_mode & S_IRWXU) != S_IRWXU) { 
        printf ("you have no right\n"); 
        rval = -4; 
        goto errout; 
    }

    printf ("and has rights\n"); 
    staletime = time (NULL) - STALE; 
    if (statbuf.st_atime < staletime || 
        statbuf.st_ctime < staletime ||
        statbuf.st_mtime < staletime) {
        printf ("i-node is too old\n"); 
        rval = -5; 
        goto errout; 
    }

    printf ("and has nearly time\n"); 
    if (uidptr != NULL)
        *uidptr = statbuf.st_uid; 
#endif

    unlink (un.sun_path); 
    return clifd; 

errout:
    close (clifd); 
    errno = err; 
    return rval; 
}

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
