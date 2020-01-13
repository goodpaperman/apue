#include "spipe.h"
#include <sys/socket.h> 
#include <sys/un.h> 
//#include <sys/stat.h> 
#include <errno.h>
#include <string.h>
#include <stddef.h> 
#include <unistd.h>
#include <stdio.h> 
#include <time.h>

//#define FIFO_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
#define CLI_PATH "/var/tmp/"
#define CLI_PERM S_IRWXU
#define QLEN 10
#define STALE 30

// note this check is mannually, not necessary for the connection establish
//#define CONN_CHECK
 
int cli_conn(const char *name)
{
    int fd, len, err, rval; 
    struct sockaddr_un un; 

    if ((fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) { 
        printf ("create socket failed\n"); 
        return -1; 
    }

    printf ("create socket ok\n"); 

#ifdef CONN_CHECK
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
#endif 

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
    int clifd, err, rval; 
    time_t staletime; 
    struct sockaddr_un un; 
    struct stat statbuf; 

    size_t len = sizeof (un); 
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

