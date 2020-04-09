#include "../apue.h"
#include "print.h"
#include "ipp.h"
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include <pthread.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/uio.h>

#define HTTP_INFO(x) ((x) >= 100 && (x) <= 199)
#define HTTP_SUCCESS(x) ((x) >= 200 && (x) <= 299)

struct job {
    struct job *next; 
    struct job *prev; 
    long jobid; 
    struct printreq req; 
}; 

struct worker_thread {
    struct worker_thread *next; 
    struct worker_thread *prev; 
    pthread_t tid; 
    int sockfd; 
}; 

int log_to_stderr = 0; 
struct addrinfo *printer; 
char *printer_name; 
pthread_mutex_t configlock = PTHREAD_MUTEX_INITIALIZER; 
int reread; 

struct worker_thread *workers; 
pthread_mutex_t workerlock = PTHREAD_MUTEX_INITIALIZER; 
sigset_t mask; 

int jobfd; 
long nextjob; 
struct job *jobhead, *jobtail; 
pthread_mutex_t joblock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t jobwait = PTHREAD_COND_INITIALIZER; 

void init_request (void)
{
    int n; 
    char name [FILENMSZ]; 
    sprintf (name, "%s/%s", SPOOLDIR, JOBFILE); 
    jobfd = open (name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); 
    if (write_lock (jobfd, 0, SEEK_SET, 0) < 0)
        log_quit ("daemon already running"); 

    if ((n = read (jobfd, name, FILENMSZ)) < 0)
        log_sys ("can't read job file"); 

    if (n == 0)
        nextjob = 1; 
    else 
        nextjob = atol (name); 
}

void init_printer (void)
{
    printer = get_printaddr (); 
    if (printer == NULL) { 
        log_msg ("no printer device registered"); 
        exit (1); 
    }

    printer_name = printer->ai_canonname; 
    if (printer_name == NULL)
        printer_name = "printer"; 

    log_msg ("printer is %s", printer_name); 
}

void update_jobno (void)
{
    char buf[FILENMSZ]; 
    lseek (jobfd, 0, SEEK_SET); 
    sprintf (buf, "%ld", nextjob); 
    if (write (jobfd, buf, strlen (buf)) < 0)
        log_sys ("can't update job file"); 
}

void add_job (struct printreq *reqp, long jobid)
{
    struct job *jp; 
    if ((jp = malloc (sizeof (struct job))) == NULL)
        log_sys ("malloc failed"); 

    memcpy (&jp->req, reqp, sizeof (struct printreq)); 
    job->jobid = jobid; 
    job->next = NULL; 

    pthread_mutex_lock (&joblock); 
    jp->prev = jobtail; 
    if (jobtail == NULL)
        jobhead = jp; 
    else 
        jobtail->next = jp; 

    jobtail = jp; 
    pthread_mutex_unlock (&joblock); 
    pthread_cond_signal (&jobwait); 
}

void build_qonstart (void)
{
    int fd, err, nr; 
    long jobid; 
    DIR *dirp; 
    struct dirent *entp; 
    struct printreq req; 
    char dname [FILENMSZ], fname [FILENMSZ]; 
    sprintf (dname, "%s/%s", SPOOLDIR, REQDIR);  
    if ((dirp = opendir (dname)) == NULL)
        return; 

    while ((entp = readdir (dirp)) != NULL) {
        if (strcmp (entp->d_name, ".") == 0 || 
                strcmp (entp->d_name, "..") == 0)
            continue; 

        sprintf (fname, "%s/%s/%s", SPOOLDIR, REQDIR, entp->d_name); 
        if ((fd = open (fname, O_RDONLY)) < 0)
            continue; 

        nr = read (fd, &req, sizeof (struct printreq)); 
        if (nr != sizeof (struct printreq)) {
            if (nr  < 0)
                err = errno; 
            else 
                err = EIO; 

            close (fd); 
            log_msg ("build_qonstart: can't read %s: %s", fname, strerror (err)); 
            unlink (fname); 
            sprintf (fname, "%s/%s/%s", SPOOLDIR, DATADIR, entp->d_name); 
            unlink (fname); 
            continue; 
        }

        jobid = atol (entp->d_name); 
        log_msg ("adding job %ld to queue", jobid); 
        add_job (&req, jobid); 
    }

    closedir (dirp); 
}

int main (int argc, char *argv[])
{
    pthread_t tid; 
    struct addrinfo *ailist, *aip; 
    int sockfd, err, i, n, maxfd; 
    int *host; 
    fd_set rendezvous, rset; 
    struct sigaction sa; 
    struct passwd *pwdp; 

    if (argc != 1)
        err_quit ("usage: printd"); 

    daemonize ("printd"); 

    sigemptyset (&sa.sa_mask); 
    sa.sa_flags = 0; 
    sa.sa_handler = SIG_IGN; 
    if (sigaction (SIGPIPE, &sa, NULL) < 0)
        log_sys ("sigaction failed"); 

    sigemptyset (&sa.sa_mask); 
    sigaddset (&mask, SIGHUP); 
    sigaddset (&mask, SIGTERM); 
    if ((err = pthread_sigmask (SIG_BLOCK, &mask, NULL)) != 0)
        log_sys ("pthread_sigmask failed"); 

    init_request (); 
    init_printer (); 

#ifdef _SC_HOST_NAME_MAX
    n = sysconf (_SC_HOST_NAME_MAX); 
    if (n < 0)
#endif
        n = HOST_NAME_MAX; 

    if ((host = malloc (n)) == NULL)
        log_sys ("malloc error"); 
    if (gethostname (host, n) < 0)
        log_sys ("gethostname error"); 

    if ((err = getaddrlist (host, "print", &ailist)) != 0) { 
        log_quit ("getaddrinfo error: %s", gai_strerror (err)); 
        exit (1); 
    }

    FD_ZERO (&rendezvous); 
    maxfd = -1; 
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver (SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) {
            FD_SET (sockfd, &rendezvous); 
            if (sockfd > maxfd)
                maxfd = sockfd; 
        }
    }

    if (maxfd == -1)
        log_quit ("service not enabled"); 

    pwdp = getpwnam("lp"); 
    if (pwdp == NULL)
        log_sys ("can't find user lp"); 
    if (pwdp->pw_uid == 0)
        log_quit ("user lp is privileged"); 
    if (setuid (pwdp->pw_uid) < 0)
        log_sys ("can't change IDs to user lp"); 

    pthread_create (&tid, NULL, printer_thread, NULL); 
    pthread_create (&tid, NULL, signal_thread, NULL); 
    build_qonstart (); 
    log_msg ("daemon initialized"); 

    for (;;) {
        rset = rendezvous; 
        if (select (maxfd + 1, &rset, NULL, NULL, NULL) < 0)
            log_sys ("select failed"); 

        for (i=0; i<=maxfd; i++) { 
            if (FD_ISSET (i, &rset)) { 
                sockfd = accept (i, NULL, NULL); 
                if (sockfd < 0)
                    log_ret ("accept failed"); 

                pthread_create (&tid, NULL, client_thread, (void *)sockfd); 
            }
        }
    }

    exit (1); 
}
