#include "spipe_fd.h" 
#include <string.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
//#include <sys/types.h>  // for ssize_t 

#define USE_CRED
#define MAXLINE 128

#if defined(__sun) || defined(sun)
#include <stropts.h> 

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

	//fprintf (stderr, "%u: send first 2 bytes ok\n", getpid ()); 
	if (fd_to_send >= 0) {
		if (ioctl (fd, I_SENDFD, fd_to_send) < 0) {
			fprintf (stderr, "%u: send fd itself failed\n", getpid ()); 
			return -1; 
		}
		else 
			;//fprintf (stderr, "%u: send fd itself ok\n", getpid ()); 
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

int recv_fd (int fd, uid_t *uidptr, ssize_t (*userfunc) (int, const void*, size_t))
{
	int newfd, nread, flag, status; 
	char *ptr; 
	char buf[MAXLINE]; 
	struct strbuf dat; 
	struct strrecvfd recvfd; 

	status = -1; 
	for (;;) { 
#if defined(__sun) || defined (sun)
		dat.buf = buf; 
		dat.maxlen = MAXLINE; 
		flag = 0; 
		if (getmsg (fd, NULL, &dat, &flag) < 0) {
			fprintf (stderr, "%u: getmsg error %d\n", getpid (), errno); 
			return -1; 
		}

		nread = dat.len; 
#else
		nread = read(fd, buf, MAXLINE); 
#endif
		if (nread == 0) {
			fprintf (stderr, "%u: connection closed by server, fd %d, errno %d\n", getpid (), fd, errno); 
			//sleep (60); 
			return -1; 
		}

		//fprintf (stderr, "%u: recv %d from peer\n", getpid (), nread); 
		for (ptr = buf; ptr < &buf[nread]; ) {
			if (*ptr ++ == 0) { 
				if (ptr != &buf[nread-1]) {
					fprintf (stderr, "%u: message format error\n", getpid ()); 
					return -1; 
				}

				//fprintf (stderr, "%u: msg format ok\n", getpid ()); 
				status = *ptr & 0xff; 
				if (status == 0) {
					if (ioctl (fd, I_RECVFD, &recvfd) < 0) {
						fprintf (stderr, "%u: recv fd itself failed\n", getpid ()); 
						return -1; 
					}

					//fprintf (stderr, "%u: recv fd itself ok, fd = %d\n", getpid (), recvfd.fd); 
					newfd = recvfd.fd; 
                    if (uidptr)
                        *uidptr = recvfd.uid; 
				}
				else {
					//fprintf (stderr, "%u: recv error code %d ok\n", getpid (), -status); 
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

#else

#include <sys/socket.h>
#include <sys/un.h>


#ifdef USE_CRED
#  if defined(SCM_CREDS) // on BSD
#  define CREDSTRUCT cmsgcred
#  define CR_UID cmcred_uid
#  define CREDOPT LOCAL_PEERCRED
#  define SCM_CREDTYPE SCM_CREDS
#  elif defined(SCM_CREDENTIALS)  // on linux
#  define CREDSTRUCT ucred
#  define CR_UID uid
#  define CREDOPT SO_PASSCRED
#  define SCM_CREDTYPE SCM_CREDENTIALS
#  else
#  error passing credentials is unsupported!
#  endif

#  define RIGHTSLEN CMSG_LEN(sizeof(int))
#  define CREDSLEN CMSG_LEN(sizeof(struct CREDSTRUCT))
#  define CONTROLLEN (RIGHTSLEN+CREDSLEN)
#else
#  define CONTROLLEN CMSG_LEN(sizeof (int))
#endif

#if 0
struct cmsghdr *my_cmsg_nxthdr (struct msghdr *__mhdr, struct cmsghdr *__cmsg)
{
  if ((size_t) __cmsg->cmsg_len < sizeof (struct cmsghdr)) {
    /* The kernel header does this so there may be a reason.  */
      fprintf (stderr, "in step1\n"); 
    return 0;
  }

  fprintf (stderr, "%p: cmsg_len %u, cmsg_level %d, cmsg_type %d\n", __cmsg, __cmsg->cmsg_len, __cmsg->cmsg_level, __cmsg->cmsg_type); 
  __cmsg = (struct cmsghdr *) (((unsigned char *) __cmsg)
			       + CMSG_ALIGN (__cmsg->cmsg_len));
  if ((unsigned char *) (__cmsg + 1) > ((unsigned char *) __mhdr->msg_control
					+ __mhdr->msg_controllen)) {
      fprintf (stderr, "in step2\n"); 
      return 0; 
  }

  fprintf (stderr, "%p: cmsg_len %u, cmsg_level %d, cmsg_type %d\n", __cmsg, __cmsg->cmsg_len, __cmsg->cmsg_level, __cmsg->cmsg_type); 
  if (((unsigned char *) __cmsg + CMSG_ALIGN (__cmsg->cmsg_len)
	  > ((unsigned char *) __mhdr->msg_control + __mhdr->msg_controllen))) {
    /* No more entries.  */
      fprintf (stderr, "in step3\n"); 
      fprintf (stderr, "msg len %d, after align %d, msg control %d\n", __cmsg->cmsg_len, CMSG_ALIGN(__cmsg->cmsg_len), __mhdr->msg_controllen); 
    return 0;
  }

  fprintf (stderr, "in final step\n"); 
  return __cmsg;
}
#endif

int send_fd (int fd, int fd_to_send)
{
    struct iovec iov[1]; 
    struct msghdr msg; 
    char buf[2]; 
#ifdef USE_CRED
    struct CREDSTRUCT *credp; 
    struct cmsghdr *cmp; 
    struct cmsghdr *cmptr = NULL; 
#endif

    iov[0].iov_base = buf; 
    iov[0].iov_len = 2; 

    msg.msg_iov = iov; 
    msg.msg_iovlen = 1; 
    msg.msg_name = NULL; 
    msg.msg_namelen = 0; 
    msg.msg_flags = 0; 

    if (fd_to_send < 0) {
        msg.msg_control = NULL; 
        msg.msg_controllen = 0; 
        buf[1] = -fd_to_send; 
        if (buf[1] == 0)
            buf[1] = 1; 
    } else {
        // add some extra space to prevent CMSG_NXTHDR return null
        int extra = 0; //5; 
        if ((cmptr = malloc(CONTROLLEN+extra)) == NULL) {
            fprintf (stderr, "malloc memory failed\n"); 
            return -1; 
        }

        // important on linux, garbage data may mess cmsg_len fields, 
        // and cause CMSG_NXTHDR return null on protection.
        memset (cmptr, 0, CONTROLLEN+extra); 
        msg.msg_control = cmptr; 
        msg.msg_controllen = CONTROLLEN+extra; 

#ifdef USE_CRED
        cmp = cmptr; 
        cmp->cmsg_level = SOL_SOCKET; 
        cmp->cmsg_type = SCM_RIGHTS; 
        cmp->cmsg_len = RIGHTSLEN; 
        *(int *) CMSG_DATA(cmp) = fd_to_send; 
        //fprintf (stderr, "add fd with len %d\n", RIGHTSLEN); 
        //fprintf (stderr, "cmsghdr = %d, cmsglen = %d, after align = %d, control len = %d\n", sizeof(struct cmsghdr), CREDSLEN, CMSG_ALIGN(CREDSLEN), CONTROLLEN); 

#  if 1
        cmp = CMSG_NXTHDR(&msg, cmp); 
#  else
        cmp = my_cmsg_nxthdr(&msg, cmp); 
#  endif
        //fprintf (stderr, "cmp = %p\n", cmp); 
        cmp->cmsg_level = SOL_SOCKET; 
        cmp->cmsg_type = SCM_CREDTYPE; 
        cmp->cmsg_len = CREDSLEN; 
        //fprintf (stderr, "add credential with len %d\n", CREDSLEN); 

        credp = (struct CREDSTRUCT *) CMSG_DATA(cmp); 
#  if defined(SCM_CREDENTIALS)
        // only linux need to set members of this struct !
        credp->uid = getuid (); 
        credp->gid = getegid (); 
        credp->pid = getpid (); 
        fprintf (stderr, "set uid %d, gid %d, pid %d\n", credp->uid, credp->gid, credp->pid);
#  endif
#else
        cmptr->cmsg_level = SOL_SOCKET; 
        cmptr->cmsg_type = SCM_RIGHTS; 
        cmptr->cmsg_len = CONTROLLEN; 

        *(int *) CMSG_DATA(cmptr) = fd_to_send; 
#endif
        buf[1] = 0; 
    }

    buf[0] = 0; 
    if (sendmsg(fd, &msg, 0) != 2) {
        free (cmptr); 
        return -1; 
    }

    free (cmptr); 
    return 0; 
}

int recv_fd (int fd, uid_t *uidptr, ssize_t (*userfunc) (int, const void*, size_t))
{
    struct cmsghdr *cmp; 
    struct CREDSTRUCT *credp; 
    const int on = -1; 

    int newfd, nr, status; 
    char *ptr; 
    char buf[MAXLINE]; 
    struct iovec iov[1]; 
    struct msghdr msg; 

    status = -1; 
    newfd = -1; 

#ifdef USE_CRED
    struct cmsghdr *cmptr = NULL; 
    if (setsockopt (fd, SOL_SOCKET, CREDOPT, &on, sizeof(int)) < 0) {
        fprintf (stderr, "setsockopt for %d failed\n", CREDOPT); 
        return -1; 
    }
#endif

    for (;;) {
        iov[0].iov_base = buf; 
        iov[0].iov_len = sizeof (buf); 

        msg.msg_iov = iov; 
        msg.msg_iovlen = 1; 
        msg.msg_name = NULL; 
        msg.msg_namelen = 0; 

        if ((cmptr = malloc (CONTROLLEN)) == NULL) {
            fprintf (stderr, "malloc error\n"); 
            return -1; 
        }

        msg.msg_control = cmptr; 
        msg.msg_controllen = CONTROLLEN; 

        if ((nr = recvmsg (fd, &msg, 0)) < 0) { 
            fprintf (stderr, "recvmsg error\n"); 
            free (cmptr); 
            return -1; 
        } else if (nr == 0) {
            fprintf (stderr, "connection closed by server\n"); 
            free (cmptr); 
            return -1; 
        }

        for (ptr = buf; ptr < &buf[nr]; ) {
            if (*ptr ++ == 0) {
                if (ptr != &buf[nr-1]) {
                    fprintf (stderr, "message format error"); 
                    free (cmptr); 
                    return -1; 
                }

                status = *ptr & 0xff; 
                if (status == 0) {
                    if (msg.msg_controllen != CONTROLLEN) { 
                        fprintf (stderr, "status = 0 but no fd\n"); 
                        free (cmptr); 
                        return -1; 
                    }

#ifdef USE_CRED
                    for (cmp = CMSG_FIRSTHDR(&msg); cmp != NULL; cmp = CMSG_NXTHDR(&msg, cmp)) { 
                        if (cmp->cmsg_level != SOL_SOCKET) {
                            fprintf (stderr, "ignore unknown socket level %d\n", cmp->cmsg_level); 
                            continue; 
                        }

                        fprintf (stderr, "msg level %d, type %d\n", cmp->cmsg_level, cmp->cmsg_type); 
                        switch (cmp->cmsg_type) {
                            case SCM_RIGHTS:
                                newfd = *(int *) CMSG_DATA(cmp); 
                                break; 
                            case SCM_CREDTYPE:
                                credp = (struct CREDSTRUCT *) CMSG_DATA(cmp); 
                                *uidptr = credp->CR_UID; 
                                break; 
                            default:
                                fprintf (stderr, "ignore unknown msg type %d\n", cmp->cmsg_type); 
                                break; 
                        }
                    }
#else
                    newfd = *(int *) CMSG_DATA(cmptr); 
#endif
                } else { 
                    newfd = -status; 
                }

                nr -= 2; 
            }
        }

        free(cmptr); 
        if (nr > 0 && (*userfunc)(STDERR_FILENO, buf, nr) != nr)
            return -1; 

        if (status >= 0)
            return newfd; 
    }

    return -1; 
}


#endif
