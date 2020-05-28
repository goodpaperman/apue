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
int reread = 0; 

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
    jobfd = open (name, O_CREAT | O_RDWR, FILEPERM); 
    if (write_lock (jobfd, 0, SEEK_SET, 0) < 0)
        log_quit ("daemon already running"); 

    if ((n = read (jobfd, name, FILENMSZ)) < 0)
        log_sys ("can't read job file"); 

    if (n == 0)
        nextjob = 1; 
    else 
        nextjob = atol (name); 

    log_msg ("nextjob: %ld", nextjob); 
}

void init_printer (void)
{
#ifdef USE_APUE_ADDRLIST
    printer = get_printaddr (); 
    if (printer == NULL) { 
        log_msg ("no printer device registered"); 
        exit (1); 
    }

    printer_name = printer->ai_canonname; 
    if (printer_name == NULL)
        printer_name = "printer"; 
#else
    printer_name = get_printname (); 
    if (printer_name == NULL) { 
        log_msg ("no printer name registered"); 
        exit (1); 
    }

#endif
    
    log_msg ("printer is %s", printer_name); 
}

int initserver (int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd; 
    int err = 0; 
    fd = socket (addr->sa_family, type, 0); 
    if (fd < 0) { 
        log_msg ("socket failed %d", errno); 
        return -1; 
    }

    int reuse = 1; 
    if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)) < 0) { 
        err = errno; 
        log_msg ("setsockopt error %d", err); 
        goto errout; 
    }

    if (bind (fd, addr, alen) < 0) { 
        err = errno; 
        log_msg ("bind error %d", err); 
        goto errout; 
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) { 
        if (listen (fd, qlen) < 0) { 
            err = errno; 
            log_msg ("listen error %d", err); 
            goto errout; 
        }
    }

    return fd; 

errout:
    close (fd); 
    errno = err; 
    return -1; 
}

void kill_workers (void)
{
    int n = 0; 
    struct worker_thread *wtp; 
    log_msg ("prepare to kill all workers, header %p", workers); 
    pthread_mutex_lock (&workerlock); 
    for (wtp = workers; wtp != NULL; wtp = wtp->next) 
    {
        // when thread cancelled later, 
        // worker nodes will be deleted from list by print_cleanup of that thread(with lock).
        log_msg ("kill worker %p", wtp); 
        pthread_cancel (wtp->tid); 
        n ++; 
    }

    pthread_mutex_unlock (&workerlock); 
    log_msg ("kill total %d workers"); 
}

void* signal_thread (void *arg)
{
    int err, signo; 
    for (;;) { 
        err = sigwait (&mask, &signo); 
        if (err == 0) { 
            log_msg ("wait signal %d", signo); 
            switch (signo) { 
                case SIGHUP:
                    pthread_mutex_lock (&configlock); 
                    reread = 1; 
                    pthread_mutex_unlock (&configlock); 
                    log_msg ("record to reread configuration next time..."); 
                    break; 
                case SIGINT:
                case SIGTERM:
                    kill_workers (); 
                    // strsignal return null ?
                    log_msg ("terminate with signal %d", signo); //strsignal (signo)); 
                    exit (0); 
                default:
                    kill_workers (); 
                    log_quit ("unexpected signal %d", signo); 
            }
        }
        else 
            log_quit ("sigwait failed: %s", strerror (err)); 
    }
}

struct job* find_job (long jobid)
{
    struct job* jp = jobhead; 
    while (jp != NULL)
    {
        if (jp->jobid == jobid)
            break; 

        jp = jp->next; 
    }

    log_msg ("find job %d: %p", jobid, jp); 
    return jp; 
}

void remove_job (struct job *target)
{
    if (target->next != NULL)
        target->next->prev = target->prev; 
    else 
        jobtail = target->prev; 

    if (target->prev != NULL)
        target->prev->next = target->next; 
    else 
        jobhead = target->next; 

    log_msg ("remove job %d", target->jobid); 
}

void update_jobno (void)
{
    char buf[FILENMSZ]; 
    lseek (jobfd, 0, SEEK_SET); 
    sprintf (buf, "%ld", nextjob); 
    if (write (jobfd, buf, strlen (buf)) < 0)
        log_sys ("can't update job file"); 
    else 
        log_msg ("update jobno to %s", buf); 
}

void replace_job (struct job *jp)
{
    pthread_mutex_lock (&joblock); 
    jp->prev = NULL; 
    jp->next = jobhead; 
    if (jobhead == NULL)
        jobtail = jp; 
    else 
        jobhead->prev = jp; 

    jobhead = jp; 
    pthread_mutex_unlock (&joblock); 
}

char* add_option (char *cp, int tag, char *optname, char *optval)
{
    int n; 
    union { 
        int16_t s; 
        char c[2]; 
    } u; 

    // TAG_ATTR
    *cp ++ = tag; 


    // OPTNAME : LEN + NAME
    n = strlen (optname); 
    u.s = htons (n); 
    *cp ++ = u.c[0]; 
    *cp ++ = u.c[1]; 

    strcpy (cp, optname); 
    cp += n; 


    // OPTVAL: LEN + VAL
    n = strlen (optval); 
    u.s = htons (n); 
    *cp ++ = u.c[0]; 
    *cp ++ = u.c[1]; 

    strcpy (cp, optval); 
    return cp + n;
}


ssize_t readmore (int sockfd, char **bpp, int off, int *bszp)
{
    ssize_t nr;
    char* bp = *bpp;
    int bsz = *bszp;
    
    if (off >= bsz) {
        bsz += IOBUFSZ;
        if ((bp = realloc(*bpp, bsz)) == NULL)
            log_sys("readmore: can't allocate bigger read buffer");

        *bszp = bsz;
        *bpp = bp;
    }

    if ((nr = tread(sockfd, &bp[off], bsz-off, 1)) > 0)
        return(off+nr);
    else
        return(-1);
}


int printer_status (int sockfd, struct job* jp)
{
    int i, success, code, len, found, bufsz; 
    long jobid; 
    ssize_t nr; 
    char *statcode, *reason, *cp, *contentlen; 
    struct ipp_hdr *hp; 
    char *bp; 

    success = 0; 
    bufsz = IOBUFSZ; 
    if ((bp = malloc (IOBUFSZ)) == NULL)
        log_sys ("printer_status: can't allocate read buffer"); 

    while ((nr = tread (sockfd, bp, IOBUFSZ, 5)) > 0) {
        cp = bp + 8; 
        while (isspace ((int) *cp))
            cp ++; 

        statcode = cp; 
        while (isdigit ((int) *cp))
            cp ++; 

        if (cp == statcode) { 
            log_msg (bp); 
        } else { 
            *cp ++ = '\0'; 
            reason = cp; 
            while (*cp != '\r' && *cp != '\n')
                cp ++; 

            *cp = '\0'; 
            code = atoi (statcode); 
            if (HTTP_INFO (code))
                continue; 

            if (!HTTP_SUCCESS (code)) { 
                bp[nr] = '\0'; 
                log_msg ("error: %s", reason); 
                break; 
            }

            i = cp - bp; 
            for (;;) {
                while (*cp != 'C' && *cp != 'c' && i < nr) { 
                    cp ++; 
                    i++; 
                }

                if (i > nr && ((nr = readmore (sockfd, &bp, i, &bufsz)) < 0))
                    goto out; 

                cp = &bp[i]; 
                if (strncasecmp (cp, "Content-Length:", 15) == 0) { 
                    cp += 15; 
                    while (isspace ((int) *cp))
                        cp ++; 

                    contentlen = cp; 
                    while (isdigit ((int) *cp))
                        cp ++; 

                    *cp ++ = '\0'; 
                    i = cp - bp; 
                    len = atoi (contentlen); 
                    break; 
                } else { 
                    cp ++; 
                    i ++; 
                }
            }

            if (i >= nr && ((nr = readmore (sockfd, &bp, i, &bufsz)) < 0))
                goto out; 

            cp = &bp[i]; 
            found = 0; 
            while (!found) { 
                while (i < nr - 2) { 
                    if (*cp == '\n' && *(cp + 1) == '\r' && *(cp + 2) == '\n') {
                        found = 1; 
                        cp += 3; 
                        i += 3; 
                        break; 
                    }

                    cp ++; 
                    i ++; 
                }

                if (i >= nr && ((nr = readmore (sockfd, &bp, i, &bufsz)) < 0))
                    goto out; 

                cp = &bp[i]; 
            }

            if (nr - i < len && ((nr = readmore (sockfd, &bp, i, &bufsz)) < 0))
                goto out; 

            cp = &bp[i]; 
            hp = (struct ipp_hdr *) cp; 
            i = ntohs (hp->status); 
            jobid = ntohl (hp->request_id); 
            if (jobid != jp->jobid) { 
                log_msg ("jobid %ld status code %d", jobid, i); 
                break; 
            }

            if (STATCLASS_OK (i))
                success = 1; 

            log_msg ("jobid %ld status %d: %s", jobid, i, ipp_err_to_msg (i)); 
            break; 
        }
    }

out:
    free (bp); 
    if (nr < 0) { 
        log_msg ("jobid %ld: error reading printer response: %s", jobid, strerror (errno)); 
    }

    return success; 
}

void* printer_thread (void *arg)
{
    struct job *jp; 
    int hlen, ilen, sockfd, fd, nr, nw; 
    char *icp, *hcp; 
    struct ipp_hdr *hp; 
    struct stat sbuf; 
    struct iovec iov[2]; 
    char name[FILENMSZ]; 
    char hbuf[HBUFSZ]; 
    char ibuf[IBUFSZ]; 
    char buf[IOBUFSZ]; 
    char str[64]; 

    for (;;) {
        pthread_mutex_lock (&joblock); 
        while (jobhead == NULL) { 
            log_msg("printer_thread: waiting..."); 
            pthread_cond_wait (&jobwait, &joblock); 
        }

        remove_job (jp = jobhead); 
        pthread_mutex_unlock (&joblock); 
        log_msg ("printer_thread: picked up job %ld", jp->jobid); 

        update_jobno ();

        pthread_mutex_lock (&configlock); 
        if (reread) { 
            freeaddrinfo (printer); 
            printer = NULL; 
            printer_name = NULL; 
            reread = 0; 
            pthread_mutex_unlock (&configlock); 
            init_printer (); 
            log_msg("re-reading config file"); 
        } else { 
            pthread_mutex_unlock (&configlock); 
        }

        sprintf (name, "%s/%s/%ld", SPOOLDIR, DATADIR, jp->jobid); 
        if ((fd = open (name, O_RDONLY)) < 0) { 
            log_msg ("job %ld canceled - can't open %s: %s", jp->jobid, name, strerror (errno)); 
            free (jp); 
            continue; 
        }

        log_msg ("open data file ok"); 
        if (fstat (fd, &sbuf) < 0) { 
            log_msg ("job %ld canceled - can't fstat %s: %s", jp->jobid, name, strerror (errno)); 
            free (jp); 
            close (fd); 
            continue; 
        }

        log_msg ("file size: %d", sbuf.st_size); 
        if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) { 
            log_msg ("job %ld deferred - can't create socket: %s", jp->jobid, strerror (errno)); 
            goto defer; 
        }

        log_msg ("create socket for printer ok, printer addr: %s", printer_name); 
#ifdef USE_APUE_ADDRLIST
        if (connect_retry (sockfd, printer->ai_addr, printer->ai_addrlen) < 0) { 
            log_msg ("job %ld deferred - can't contact printer: %s", jp->jobid, strerror (errno)); 
            goto defer; 
        }
#else
        struct sockaddr_in addr = { 0 }; 
        addr.sin_family = AF_INET; 
        addr.sin_addr.s_addr = inet_addr (printer_name); 
        addr.sin_port = htons (IPP_PORT); 
        size_t addrlen = sizeof (addr); 
        if (connect_retry (sockfd, (struct sockaddr *) &addr, addrlen) < 0) { 
            log_msg ("job %ld deferred - can't contact printer: %s", jp->jobid, strerror (errno)); 
            goto defer; 
        }
#endif

        log_msg ("connect to printer ok"); 

        icp = ibuf; 
        hp = (struct ipp_hdr *)icp; 
        hp->major_version = 1; 
        hp->minor_version = 1; 
        hp->operation = htons (OP_PRINT_JOB); 
        hp->request_id = htonl (jp->jobid); 
        icp += offsetof (struct ipp_hdr, attr_group); 
        *icp ++ = TAG_OPERATION_ATTR; 
        icp = add_option (icp, TAG_CHARSET, "attributes-charset", "utf-8"); 
        icp = add_option (icp, TAG_NATULANG, "attributes-natural-language", "en-us"); 
        sprintf (str, "http://%s:%d", printer_name, IPP_PORT); 
        icp = add_option (icp, TAG_URI, "printer-uri", str); 
        icp = add_option (icp, TAG_NAMEWOLANG, "requesting-user-name", jp->req.usernm); 
        icp = add_option (icp, TAG_NAMEWOLANG, "job-name", jp->req.jobnm); 
        if (jp->req.flags & PR_TEXT) { 
            icp = add_option (icp, TAG_MIMETYPE, "document-format", "text/plain"); 
        } else { 
            icp = add_option (icp, TAG_MIMETYPE, "document-format", "application/postscript"); 
        }

        *icp ++ = TAG_END_OF_ATTR; 
        ilen = icp - ibuf; 

        hcp = hbuf; 
        sprintf (hcp, "POST /%s/ipp HTTP/1.1\r\n", printer_name); 
        hcp += strlen (hcp); 
        sprintf (hcp, "Content-Length: %ld\r\n", (long) sbuf.st_size + ilen); 
        hcp += strlen (hcp); 
        strcpy (hcp, "Content-Type: application/ipp\r\n"); 
        hcp += strlen (hcp); 
        sprintf (hcp, "Host: %s:%d\r\n", printer_name, IPP_PORT); 
        hcp += strlen (hcp); 
        *hcp ++ = '\r'; 
        *hcp ++ = '\n'; 
        hlen = hcp - hbuf; 

        iov[0].iov_base = hbuf; 
        iov[0].iov_len = hlen; 
        iov[1].iov_base = ibuf; 
        iov[1].iov_len = ilen; 
        if ((nw = writev (sockfd, iov, 2)) != hlen + ilen) { 
            log_ret ("can't write to printer"); 
            goto defer; 
        }

        log_msg ("write to printer ipp protocol ok"); 
        while ((nr = read (fd, buf, IOBUFSZ)) > 0) { 
            log_msg ("read file data %d", nr); 
            if ((nw = write (sockfd, buf, nr)) != nr) { 
                if (nw < 0)
                    log_ret ("can't write to printer"); 
                else 
                    log_msg ("short write (%d/%d) to printer", nw, nr); 

                goto defer; 
            }
            else 
                log_msg ("write to printer ipp data %d", nw); 
        }

        if (nr < 0) { 
            log_ret ("can't read %s", name); 
            goto defer; 
        }

        if (printer_status (sockfd, jp)) { 
            unlink (name); 
            sprintf (name, "%s/%s/%ld", SPOOLDIR, REQDIR, jp->jobid); 
            unlink (name); 
            free (jp); 
            jp = NULL; 
        }

defer:
        close (fd); 
        if (sockfd >= 0) 
            close (sockfd); 
        if (jp != NULL) { 
            replace_job (jp); 
            sleep (60); 
        }
    }
}

void add_job (struct printreq *reqp, long jobid)
{
    struct job *jp; 
    if ((jp = malloc (sizeof (struct job))) == NULL)
        log_sys ("malloc failed"); 

    memcpy (&jp->req, reqp, sizeof (struct printreq)); 
    jp->jobid = jobid; 
    jp->next = NULL; 

    pthread_mutex_lock (&joblock); 
    jp->prev = jobtail; 
    if (jobtail == NULL)
        jobhead = jp; 
    else 
        jobtail->next = jp; 

    jobtail = jp; 
    pthread_mutex_unlock (&joblock); 
    log_msg ("add job %ld to queue", jobid); 
    pthread_cond_signal (&jobwait); 
}

void build_qonstart (void)
{
    int fd, err, nr, total = 0; 
    long jobid; 
    DIR *dirp; 
    struct dirent *entp; 
    struct printreq req; 
    char dname [FILENMSZ], fname [FILENMSZ]; 
    sprintf (dname, "%s/%s", SPOOLDIR, REQDIR);  
    if ((dirp = opendir (dname)) == NULL) {
        log_msg ("opendir %s failed", dname); 
        return; 
    }

    while ((entp = readdir (dirp)) != NULL) {
        if (strcmp (entp->d_name, ".") == 0 || 
                strcmp (entp->d_name, "..") == 0)
            continue; 

        sprintf (fname, "%s/%s/%s", SPOOLDIR, REQDIR, entp->d_name); 
        if ((fd = open (fname, O_RDONLY)) < 0) {
            log_msg ("open req file failed: %s", fname); 
            continue; 
        }

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
        add_job (&req, jobid); 
        total ++; 
    }

    closedir (dirp); 
    log_msg("add total %d jobs from dir", total); 
}

void print_cleanup (void *arg)
{
    struct worker_thread *wtp; 
    pthread_t tid = (pthread_t) arg; 
    log_msg ("prepare to cleanup client when thread exit"); 
    pthread_mutex_lock (&workerlock); 
    for (wtp = workers; wtp != NULL; wtp = wtp->next) { 
        if (wtp->tid == tid) { 
            if (wtp->next != NULL)
                wtp->next->prev = wtp->prev; 
            if (wtp->prev != NULL)
                wtp->prev->next = wtp->next; 
            else 
                // is the HEAD
                workers = wtp->next; 

            break; 
        }
    }

    pthread_mutex_unlock (&workerlock); 
    if (wtp != NULL) { 
        log_msg ("clear worker %p, header now %p", wtp, workers); 
        close (wtp->sockfd); 
        free (wtp); 
    }
}

void add_worker (pthread_t tid, int sockfd)
{
    struct worker_thread *wtp; 
    if ((wtp = malloc (sizeof (struct worker_thread))) == NULL) { 
        log_ret ("add_worker: can't malloc"); 
        pthread_exit ((void *)1); 
    }

    wtp->tid = tid; 
    wtp->sockfd = sockfd; 

    log_msg ("prepare to add worker"); 
    pthread_mutex_lock (&workerlock); 
#if 0
    wtp->prev = NULL; 
    wtp->next = workers; 
    if (workers == NULL)
        workers = wtp; 
    else 
        workers->prev = wtp; 
#else
    // for single list, we can only insert head
#  if 0
    // too complex...
    wtp->prev = workers; 
    wtp->next = workers ? workers->next : NULL; 
    if (workers == NULL)
        workers = wtp; 
    else { 
        if (workers->next == NULL)
            workers->next = wtp; 
        else {
            workers->next->prev = wtp; 
            workers->next = wtp; 
        }
    }
#  else
    wtp->prev = NULL; 
    wtp->next = workers; 
    if (workers != NULL)
        workers->prev = wtp; 

    workers = wtp; 
#  endif
#endif

    pthread_mutex_unlock (&workerlock); 
    log_msg ("add worker %p for client %d, header now %p", wtp, sockfd, workers); 
}

long get_newjobno (void)
{
    long jobid; 
    pthread_mutex_lock (&joblock); 
    jobid = nextjob ++; 
    if (nextjob <= 0)
        nextjob = 1; 
    pthread_mutex_unlock (&joblock); 
    log_msg ("got new jobno: %d", jobid); 
    return jobid; 
}

void* print_client (void *arg)
{
    int n, fd, sockfd, nr, nw, first; 
    long jobid; 
    pthread_t tid; 
    struct printreq req; 
    struct printresp res; 
    char name[FILENMSZ]; 
    char buf[IOBUFSZ]; 

    tid = pthread_self (); 
    pthread_cleanup_push (print_cleanup, (void *)tid); 
    sockfd = (int)arg; 
    add_worker (tid, sockfd); 

    if ((n = treadn (sockfd, &req, sizeof (struct printreq), 10)) != sizeof(struct printreq)) {
        res.jobid = 0; 
        if (n < 0)
            res.retcode = htonl (errno); 
        else 
            res.retcode = htonl (EIO); 

        strncpy (res.msg, strerror (errno), MSGLEN_MAX); 
        writen (sockfd, &res, sizeof (struct printresp)); 
        log_msg ("readn failed, send back client err %d", ntohl (res.retcode)); 
        pthread_exit ((void *)1); 
    }

    log_msg ("got request data: %d", n); 
    req.size = ntohl (req.size); 
    req.flags = ntohl (req.flags); 

    jobid = get_newjobno (); 
    sprintf (name, "%s/%s/%ld", SPOOLDIR, DATADIR, jobid); 
    log_msg ("got a new request: \n"
             "    size: %ld\n"
             "    flag: %d\n"
             "    name: %s", 
             req.size, 
             req.flags, 
             name); 

    if ((fd = creat (name, FILEPERM)) < 0) {
        res.jobid = 0; 
        res.retcode = htonl (errno); 
        log_msg ("client thread: can't create %s: %s", name, strerror (errno)); 
        strncpy (res.msg, strerror (errno), MSGLEN_MAX); 
        writen (sockfd, &res, sizeof (struct printresp)); 
        pthread_exit ((void *)1); 
    }
    else
        log_msg ("create job file %s", name); 

    first = 1; 
    while ((nr = tread (sockfd, buf, IOBUFSZ, 20)) > 0) { 
        if (first) { 
            first = 0; 
            if (strncmp (buf, "%!PS", 4) != 0)
                req.flags |= PR_TEXT; 
        }

        log_msg ("got file data %d", nr); 
        nw = write (fd, buf, nr); 
        if (nw != nr) { 
            if (nw < 0)
                res.retcode = htonl (errno); 
            else 
                res.retcode = htonl (EIO); 

            log_msg ("print_client: can't write %s: %s", name, strerror (errno)); 
            strncpy (res.msg, strerror (errno), MSGLEN_MAX); 
            writen (sockfd, &res, sizeof (struct printresp)); 

            close (fd); 
            unlink (name); 
            pthread_exit ((void *)1); 
        }

        log_msg ("write into file %d", nw); 
    }

    close (fd); 
    sprintf (name, "%s/%s/%ld", SPOOLDIR, REQDIR, jobid); 
    fd = creat (name, FILEPERM); 
    if (fd < 0) { 
        res.jobid = 0; 
        res.retcode = htonl (errno); 
        log_msg ("print_client: can't create %s: %s", name, strerror (errno)); 
        strncpy (res.msg, strerror (errno), MSGLEN_MAX); 
        writen (sockfd, &res, sizeof (struct printresp)); 

        close (fd); 
        unlink (name); 
        sprintf (name, "%s/%s/%ld", SPOOLDIR, DATADIR, jobid); 
        unlink (name); 
        pthread_exit ((void *)1); 
    }
    else 
        log_msg ("create request file %s", name); 

    nw = write (fd, &req, sizeof (struct printreq)); 
    if (nw != sizeof (struct printreq))
    {
        res.jobid = 0; 
        if (nw < 0)
            res.retcode = htonl (errno); 
        else 
            res.retcode = htonl (EIO); 

        log_msg ("print_client: can't write %s: %s", name, strerror (res.retcode)); 
        strncpy (res.msg, strerror (res.retcode), MSGLEN_MAX); 
        writen (sockfd, &res, sizeof (struct printresp)); 

        close (fd); 
        unlink (name); 
        sprintf (name, "%s/%s/%ld", SPOOLDIR, DATADIR, jobid); 
        unlink (name); 
        pthread_exit ((void *)1); 
    }

    close (fd); 
    res.retcode = 0; 
    res.jobid = htonl (jobid); 
    sprintf (res.msg, "request ID %ld", jobid); 
    nr = writen (sockfd, &res, sizeof (struct printresp)); 
    log_msg ("send back client response: %d", nr); 

    add_job (&req, jobid); 
    pthread_cleanup_pop (1); 
    return ((void *)0); 
}

void* hang_client (void *arg)
{
    int n, sockfd, nr, nw; 
    struct hangreq req; 
    struct hangresp res; 
    char name[FILENMSZ]; 
    char buf[IOBUFSZ]; 
    long jobid; 

    sockfd = (int)arg; 
    if ((n = treadn (sockfd, &req, sizeof (struct hangreq), 10)) != sizeof(struct hangreq)) {
        if (n < 0)
            res.retcode = htonl (errno); 
        else 
            res.retcode = htonl (EIO); 

        strncpy (res.msg, strerror (errno), MSGLEN_MAX); 
        writen (sockfd, &res, sizeof (struct printresp)); 
        log_msg ("readn failed, send back client err %d", ntohl (res.retcode)); 
        pthread_exit ((void *)1); 
    }

    log_msg ("got request data: %d", n); 

    jobid = ntohl(req.jobid); 
    pthread_mutex_lock (&joblock); 
    if (jobid == 0)
    {
        //list all hanging jobs
        int n = 0; 
        struct job* jp; 
        for (jp = jobhead; jp != NULL; jp = jp->next)
        {
            if (strcasecmp (req.usernm, "root") == 0 ||
                strcasecmp (req.usernm, jp->req.usernm) == 0)
            {
                // only list files issued by himself
                n ++; 
                sprintf (res.msg + strlen (res.msg), "  %ld, %s : %s (%u)\n", jp->jobid, jp->req.usernm, jp->req.jobnm, jp->req.size); 
            }
        }

        res.retcode = 0; 
        log_msg ("list total %d jobs\n", n); 
    }
    else
    {
        //cancel this job, first find it..
        struct job* jp = find_job (jobid); 
        if (jp == NULL)
        {
            res.retcode = -1; 
            sprintf (res.msg, "can not find that job"); 
        }
        else 
        {
            if (!(strcasecmp (req.usernm, "root") == 0 ||
                  strcasecmp (req.usernm, jp->req.usernm) == 0))
            {
                // only cancel files issued by himself
                res.retcode = -2; 
                sprintf (res.msg, "can not hang files issued by other user!"); 
            }
            else
            {
                res.retcode = 0; 
                remove_job (jp); 
                sprintf (res.msg, "find and remove that job"); 
                sprintf (name, "%s/%s/%ld", SPOOLDIR, DATADIR, jobid); 
                unlink (name); 
                sprintf (name, "%s/%s/%ld", SPOOLDIR, REQDIR, jobid); 
                unlink (name); 
            }
        }
    }

    pthread_mutex_unlock (&joblock); 
    nr = writen (sockfd, &res, sizeof (struct hangresp)); 
    log_msg ("send back client response: %d", nr); 
    return ((void *)0); 
}

int main (int argc, char *argv[])
{
    pthread_t tid; 
    struct addrinfo *ailist, *aip; 
    int fd, sockfd, err, i, n, maxfd; 
    fd_set rendezvous, rset; 
    struct sigaction sa; 
    struct passwd *pwdp; 

    if (argc != 1)
        err_quit ("usage: printd"); 

    daemonize ("printd"); 
    log_msg ("daemonize ok"); 

    sigemptyset (&sa.sa_mask); 
    sa.sa_flags = 0; 
    sa.sa_handler = SIG_IGN; 
    if (sigaction (SIGPIPE, &sa, NULL) < 0)
        log_sys ("sigaction failed"); 

    sigemptyset (&sa.sa_mask); 
    sigaddset (&mask, SIGHUP); 
    sigaddset (&mask, SIGTERM); 
    sigaddset (&mask, SIGINT); 
    if ((err = pthread_sigmask (SIG_BLOCK, &mask, NULL)) != 0)
        log_sys ("pthread_sigmask failed"); 

#ifdef _SC_HOST_NAME_MAX
    n = sysconf (_SC_HOST_NAME_MAX); 
    if (n < 0)
#endif
        n = HOST_NAME_MAX; 

    FD_ZERO (&rendezvous); 
    maxfd = -1; 
#ifdef USE_APUE_ADDRLIST
    int *host; 
    if ((host = malloc (n)) == NULL)
        log_sys ("malloc error"); 

    if (gethostname (host, n) < 0)
        log_sys ("gethostname error"); 

    log_msg ("host: %s", host); 
    if ((err = getaddrlist (host, "print", &ailist)) != 0) { 
        log_quit ("getaddrinfo error: %s", gai_strerror (err)); 
        exit (1); 
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver (SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) {
            FD_SET (sockfd, &rendezvous); 
            if (sockfd > maxfd)
                maxfd = sockfd; 
        }
    }
#else
    struct sockaddr_in addr = { 0 }; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = INADDR_ANY; 
    addr.sin_port = htons (PRINTSVC_PORT); 

    size_t addrlen = sizeof (addr); 
    //inet_aton (printer_name, &addr.sin_addr); 
    if ((sockfd = initserver (SOCK_STREAM, (struct sockaddr *) &addr, addrlen, QLEN)) >= 0) {
        FD_SET (sockfd, &rendezvous); 
        if (sockfd > maxfd)
            maxfd = sockfd; 

        log_msg ("init print server fd: %d, port %d", sockfd, PRINTSVC_PORT); 
    }

    addr.sin_port = htons (HANGSVC_PORT); 
    addrlen = sizeof (addr); 
    if ((sockfd = initserver (SOCK_STREAM, (struct sockaddr *) &addr, addrlen, QLEN)) >= 0) {
        FD_SET (sockfd, &rendezvous); 
        if (sockfd > maxfd)
            maxfd = sockfd; 

        log_msg ("init hang server fd: %d, port %d", sockfd, HANGSVC_PORT); 
    }
#endif

    if (maxfd == -1)
        log_quit ("service not enabled"); 

    pwdp = getpwnam("lp"); 
    if (pwdp == NULL)
        log_sys ("can't find user lp"); 

    if (pwdp->pw_uid == 0)
        log_quit ("user lp is privileged"); 

    if (setuid (pwdp->pw_uid) < 0)
        log_sys ("can't change IDs to user lp"); 
    else 
        log_msg ("change user to lp: %d", pwdp->pw_uid); 

    init_request (); 
    // no lock here, as no other thread created yet, we are single thread~
    init_printer (); 

    pthread_create (&tid, NULL, printer_thread, NULL); 
    pthread_create (&tid, NULL, signal_thread, NULL); 
    build_qonstart (); 
    
    log_msg ("daemon initialized"); 
    for (;;) {
        rset = rendezvous; 
        if (select (maxfd + 1, &rset, NULL, NULL, NULL) < 0)
            log_sys ("select failed"); 
        else
            log_msg ("select return"); 

        for (i=0; i<=maxfd; i++) { 
            if (FD_ISSET (i, &rset)) { 
                if (i == sockfd) { 
                    // hang server
                    log_msg ("hang listen fd %d has event", i); 
                    fd = accept (i, NULL, NULL); 
                    if (fd < 0)
                        log_ret ("accept failed"); 
                    else 
                        log_msg ("accept new client %d", fd); 

                    pthread_create (&tid, NULL, hang_client, (void *)fd); 
                }
                else 
                {
                    // print server
                    log_msg ("print listen fd %d has event", i); 
                    fd = accept (i, NULL, NULL); 
                    if (fd < 0)
                        log_ret ("accept failed"); 
                    else 
                        log_msg ("accept new client %d", fd); 

                    pthread_create (&tid, NULL, print_client, (void *)fd); 
                }
            }
        }
    }

    exit (1); 
}

