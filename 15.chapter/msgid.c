#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL
//#define USE_NOWAIT
//#define USE_NOERR

//#define DUMP_QUEUE
#define MAX_DATA_SIZE 512

#include <sys/msg.h>

struct msgbuf { 
    long type; 
    char data[MAX_DATA_SIZE]; 
}; 

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
    int i=0, mid=0, ret=0; 
    key_t key = IPC_PRIVATE; 
    for (i=0; i<10; ++ i)
    {
        if (i < 5)
            key = ftok ("./msgid.c", i); 
        else 
            key = IPC_PRIVATE;

        int mode = 0666; 
        int flag = IPC_CREAT; 
#ifdef USE_EXCL
        flag |= IPC_EXCL; 
#endif

        mid = msgget (key, flag | mode); 
        if (mid < 0)
            err_sys ("msgget for key %ld failed", key); 

        printf ("create msgqueue %d with key 0x%08x ok\n", mid, key); 
        //dump_queue ("after open: ", mid, 1); 

        if (i < 5)
        {
            ret = msgctl (mid, IPC_RMID, NULL); 
            if (ret < 0)
                err_sys ("msgctl to remove queue failed"); 

            printf ("remove queue %d\n", mid); 
        }
        else 
        {
            struct msgbuf buf = { 0 }; 
            int flag = 0; 
#ifdef USE_NOWAIT
            flag |= IPC_NOWAIT; 
#endif
#ifdef USE_NOERR
            flag |= MSG_NOERROR; 
#endif
            // write
            buf.type = i+1; 
            sprintf (buf.data, "this is msg %d", i+1); 
            ret = msgsnd (mid, &buf, strlen (buf.data), flag); 
            if (ret < 0)
                printf ("send msg failed, ret %d, errno %d\n", ret, errno); 
            else 
                printf ("send %d, type %ld\n", ret, buf.type); 
        }
    }

    return 0; 
}
