#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL
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

void dump_queue (int mid)
{
    struct msqid_ds mbuf; 
    int ret = msgctl (mid, IPC_STAT, &mbuf); 
    if (ret < 0)
        err_sys ("msgctl to stat queue failed"); 

    printf ("queue %d: \n"
            "   qnum: %d\n"
            "   qbytes: %d\n"
            "   last send pid: %d\n"
            "   last recv pid: %d\n"
            "   last send time: %d\n"
            "   last recv time: %d\n"
            "   last change time: %d\n", 
            mid, 
            mbuf.msg_qnum, 
            mbuf.msg_qbytes, 
            mbuf.msg_lspid, 
            mbuf.msg_lrpid, 
            mbuf.msg_stime, 
            mbuf.msg_rtime, 
            mbuf.msg_ctime
            ); 

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
        err_sys ("msgget failed"); 

    printf ("create msgqueue %d with key 0x%08x ok\n", mid, key); 

    // after set mode bits in msgget, we don't need do this again.
    //// read access right is always needed as dumping queue, event for write process
    //set_mode (mid, S_IRUSR | (put == 1 ? S_IWUSR : 0)); 
    dump_queue (mid); 

    ssize_t res = 0; 
    struct msgbuf buf = { 0 }; 
    for (n=0; n<MAX_QUEUE_SIZE; ++ n)
    {
        if (put == 0)
        {
            // read
            res = msgrcv (mid, &buf, sizeof (buf.data), 0, 0); 
            if (res < 0)
            {
                printf ("receive msg failed, ret %d, errno %d\n", res, errno); 
                break; 
            }

            printf ("recv %d, type %ld, content %s\n", res, buf.type, buf.data); 
        }
        else 
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
        }
    }

    printf ("operate queue over\n"); 
    ret = msgctl (mid, IPC_RMID, NULL); 
    if (ret < 0)
        err_sys ("msgctl to remove queue failed"); 

    printf ("remove that queue\n"); 
    return 0; 
}
