#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL
//#define USE_TYPE 1
//#define USE_NOWAIT
#define USE_E2BIG
//#define USE_NOERR

#define READ_REMOVEQ
#define WRITE_REMOVEQ
//#define DUMP_QUEUE
#define MAX_QUEUE_SIZE 10
#define MAX_DATA_SIZE 512

#include <sys/msg.h>

struct msgbuf { 
    long type; 
    char data[MAX_DATA_SIZE]; 
}; 

void set_mode (int mid, int mode)
{
    struct msqid_ds mbuf; 
    mbuf.msg_perm.uid = getpid (); 
    mbuf.msg_perm.gid = getpgid (getpid ()); 
    mbuf.msg_perm.mode = mode; 
    mbuf.msg_qbytes = 0; 
    int ret = msgctl (mid, IPC_SET, &mbuf); 
    if (ret < 0)
        err_sys ("msgctl to stat queue failed"); 

    printf ("set queue mode ok\n"); 
}

void dump_perm (struct ipc_perm* perm)
{
    printf ("perm: \n"
            "   uid: %d\n"
            "   gid: %d\n"
            "   create uid: %d\n"
            "   create gid: %d\n"
            "   mode: 0x%08x\n"
            "   key: 0x%08x\n"
            "   seq: %d\n", 
            perm->uid, 
            perm->gid, 
            perm->cuid, 
            perm->cgid, 
            perm->mode, 
            perm->__key, 
            perm->__seq); 
}

void dump_queue (char const *prompt, int mid, int with_perm)
{
    struct msqid_ds mbuf; 
    int ret = msgctl (mid, IPC_STAT, &mbuf); 
    if (ret < 0)
        err_sys ("msgctl to stat queue failed"); 

    printf ("%s queue %d: \n"
            "   qnum: %d\n"
            "   qbytes: %d\n"
            "   last send pid: %d\n"
            "   last recv pid: %d\n"
            "   last send time: %d\n"
            "   last recv time: %d\n"
            "   last change time: %d\n", 
            prompt, 
            mid, 
            mbuf.msg_qnum, 
            mbuf.msg_qbytes, 
            mbuf.msg_lspid, 
            mbuf.msg_lrpid, 
            mbuf.msg_stime, 
            mbuf.msg_rtime, 
            mbuf.msg_ctime
            ); 

    if (with_perm)
        dump_perm (&mbuf.msg_perm); 
}

int main (int argc, char *argv[])
{
    int projid = -1; 
    int put = 0;  // 0: get; 1: put
    if (argc > 1)
    {
        //printf ("Usage: xsique <projid> \n"); 
        projid = atoi (argv[1]); 
    }

    if (argc > 2)
    {
        put = (strcasecmp (argv[2], "get") == 0 ? 0 : 1); 
        printf ("mode: %s\n", put == 0 ? "get" : "put"); 
    }

    int mid = 0; 
    int ret = 0, n = 0; 
    if (argc > 3)
    {
        mid = atol (argv[3]); 
        printf ("use mid %d\n", mid); 
    }
    else 
    {
        key_t key = IPC_PRIVATE; 
        if (projid >= 0)
            key = ftok ("./xsique.c", projid); 

        int mode = 0666; 
        int flag = IPC_CREAT; 
#ifdef USE_EXCL
        flag |= IPC_EXCL; 
#endif

        mid = msgget (key, flag | mode); 
        if (mid < 0)
            err_sys ("msgget for key %ld failed", key); 

        printf ("create msgqueue %d with key 0x%08x ok\n", mid, key); 
    }

    // after set mode bits in msgget, we don't need do this again.
    //// read access right is always needed as dumping queue, event for write process
    //set_mode (mid, S_IRUSR | (put == 1 ? S_IWUSR : 0)); 
    dump_queue ("after open: ", mid, 1); 

    ssize_t res = 0; 
    struct msgbuf buf = { 0 }; 
    size_t size = 0; 

    int flag = 0; 
#ifdef USE_NOWAIT
    flag |= IPC_NOWAIT; 
#endif
#ifdef USE_NOERR
    flag |= MSG_NOERROR; 
#endif
    if (put == 0)
    {
#if USE_TYPE == 1
        n = MAX_QUEUE_SIZE; 
#elif USE_TYPE == 2
        n = -MAX_QUEUE_SIZE / 2; 
#else
        n = 0; 
#endif

#if defined(USE_E2BIG) || defined (USE_NOERR)
        // make error condition
        size = 1; 
#else
        size = sizeof (buf.data); 
#endif

        while (1)
        {
            // read
#ifdef USE_TYPE
            printf ("require type %d\n", n); 
#endif
            res = msgrcv (mid, &buf, size, n, flag); 
            if (res < 0)
            {
                printf ("receive msg failed, ret %d, errno %d\n", res, errno); 
#ifdef USE_NOWAIT
                if (errno == ENOMSG)
                {
                    printf ("retry 1 second later for ENOMSG\n"); 
                    sleep (1); 
                    continue; 
                }
#endif

#ifdef USE_E2BIG
                if (errno == E2BIG)
                {
                    if (size == sizeof (buf.data))
                    {
                        printf ("message too large, fatal error\n"); 
                        break; 
                    }

                    size *= 2; 
                    if (size > sizeof (buf.data))
                        size = sizeof (buf.data); 

                    printf ("message buffer too small, try to enlarge it to %d\n", size); 
                    continue; 
                }
#endif 
                break; 
            }
            else if (res == 0)
            {
                printf ("receive end message, type %ld, quit\n", buf.type); 
#ifdef DUMP_QUEUE
                dump_queue ("after recv: ", mid, 0); 
#endif
                break; 
            }

#ifdef USE_NOERR
            size *= 2; 
            if (size > sizeof (buf.data))
                size = sizeof (buf.data); 
#endif

            buf.data[res] = 0;  // null terminated
            printf ("recv %d, type %ld: %s\n", res, buf.type, buf.data); 
#ifdef DUMP_QUEUE
            dump_queue ("after recv: ", mid, 0); 
            sleep (1); 
#endif 

#if USE_TYPE == 1
            n--; 
#elif USE_TYPE == 2
            n++; 
#endif
        }
    }
    else 
    {
#ifdef USE_NOWAIT
        for (n=0;;++n)
#else
        for (n=0; n<MAX_QUEUE_SIZE; ++ n)
#endif
        {
            // write
            buf.type = n+1; 
            sprintf (buf.data, "this is msg %d", n+1); 
            res = msgsnd (mid, &buf, strlen (buf.data), flag); 
            if (res < 0)
            {
                printf ("send msg failed, ret %d, errno %d\n", res, errno); 
#ifdef USE_NOWAIT
                if (errno == EAGAIN)
                {
                    printf ("retry 1 second later for EAGAIN\n"); 
                    sleep (1); 
                    continue; 
                }
#endif
                break; 
            }

            printf ("send %d, type %ld\n", res, buf.type); 
#ifdef DUMP_QUEUE
            dump_queue ("after send: ", mid, 0); 
            sleep (1); 
#endif
        }

        // end with a empty message.
        buf.type = 1; 
        res = msgsnd (mid, &buf, 0, flag); 
        if (res < 0)
            printf ("send empty msg failed, ret %d, errno %d\n", res, errno); 
        else
        {
            printf ("send end message\n"); 
#ifdef DUMP_QUEUE
            dump_queue ("after send: ", mid, 0); 
            sleep (1); 
#endif
        }

    }

    printf ("operate queue over\n"); 

    switch (put)
    {
        case 0:
#ifdef READ_REMOVEQ
            goto DEFAULT; 
#else 
            break; 
#endif
        case 1:
#ifdef WRITE_REMOVEQ
            goto DEFAULT; 
#else
            break; 
#endif
DEFAULT:
        default:
            {
                ret = msgctl (mid, IPC_RMID, NULL); 
                if (ret < 0)
                    err_sys ("msgctl to remove queue failed"); 

                printf ("remove that queue\n"); 
                break; 
            }
    }

    return 0; 
}
