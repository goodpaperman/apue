#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL
#define USE_TYPE 1
#define READ_REMOVEQ
//#define WRITE_REMOVEQ
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
            "   mode: 0x%08x\n", 
            perm->uid, 
            perm->gid, 
            perm->cuid, 
            perm->cgid, 
            perm->mode); 
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
        printf ("%s put\n", put == 0 ? "get" : "put"); 
    }

    key_t key = IPC_PRIVATE; 
    if (projid >= 0)
        key = ftok ("./xsique.c", projid); 

    int mode = 0666; 
    int flag = IPC_CREAT; 
#ifdef USE_EXCL
    flag |= IPC_EXCL; 
#endif

    int ret = 0, n = 0; 
    int mid = msgget (key, flag | mode); 
    if (mid < 0)
        err_sys ("msgget for key %ld failed", key); 

    printf ("create msgqueue %d with key 0x%08x ok\n", mid, key); 

    // after set mode bits in msgget, we don't need do this again.
    //// read access right is always needed as dumping queue, event for write process
    //set_mode (mid, S_IRUSR | (put == 1 ? S_IWUSR : 0)); 
    dump_queue ("after open: ", mid, 1); 

    ssize_t res = 0; 
    struct msgbuf buf = { 0 }; 
    if (put == 0)
    {
#if USE_TYPE == 1
        n = MAX_QUEUE_SIZE; 
#elif USE_TYPE == 2
        n = -MAX_QUEUE_SIZE / 2; 
#else
        n = 0; 
#endif
        while (1)
        {
            // read
            printf ("require type %d\n", n); 
            res = msgrcv (mid, &buf, sizeof (buf.data), n, 0); 
            if (res < 0)
            {
                printf ("receive msg failed, ret %d, errno %d\n", res, errno); 
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

            buf.data[res] = 0;  // null terminated
            printf ("recv %d, type %ld, content %s\n", res, buf.type, buf.data); 
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
        for (n=0; n<MAX_QUEUE_SIZE; ++ n)
        {
            // write
            buf.type = n+1; 
            sprintf (buf.data, "this is msg %d", n+1); 
            res = msgsnd (mid, &buf, strlen (buf.data), 0); 
            if (res < 0)
            {
                printf ("send msg failed, ret %d, errno %d\n", res, errno); 
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
        res = msgsnd (mid, &buf, 0, 0); 
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
#ifdef WRITE_REMOVEQ
    if (put == 1)
#elif defined(READ_REMOVEQ)
    if (put == 0)
#else 
    if (0)
#endif
    {
        ret = msgctl (mid, IPC_RMID, NULL); 
        if (ret < 0)
            err_sys ("msgctl to remove queue failed"); 

        printf ("remove that queue\n"); 
    }

    return 0; 
}
