#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL
//#define USE_TYPE 1
#define SEM_INIT

#define READ_REMOVES
#define WRITE_REMOVES
//#define DUMP_QUEUE
//#define MAX_QUEUE_SIZE 10
//#define MAX_DATA_SIZE 512

#include <sys/sem.h>

union semun
{
  int val;				//<= value for SETVAL
  struct semid_ds *buf; //		<= buffer for IPC_STAT & IPC_SET
  unsigned short int *array;// 		<= array for GETALL & SETALL
  struct seminfo *__buf;    //		<= buffer for IPC_INFO
};


void set_mode (int mid, int n, int mode)
{
    struct semid_ds mbuf; 
    mbuf.sem_perm.uid = getpid (); 
    mbuf.sem_perm.gid = getpgid (getpid ()); 
    mbuf.sem_perm.mode = mode; 

    union semun sem; 
    sem.buf = &mbuf; 
    int ret = semctl (mid, n, IPC_SET, sem); 
    if (ret < 0)
        err_sys ("semctl to set mode for %d failed", n); 

    printf ("set sem mode ok\n"); 
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

void dump_sem (char const *prompt, int mid, int size, int with_perm)
{
    struct semid_ds mbuf; 
    union semun sem; 
    sem.buf = &mbuf; 
    int ret = semctl (mid, 0, IPC_STAT, sem); 
    if (ret < 0)
        err_sys ("semctl to stat sem failed"); 

    printf ("%s: \n"
            "   sem id: %d\n"
            "   number sems: %d\n"
            "   last semop time: %d\n"
            "   last change time: %d\n", 
            prompt, 
            mid, 
            mbuf.sem_nsems, 
            mbuf.sem_otime, 
            mbuf.sem_ctime
            ); 

    int n = 0; 
    printf ("   \n"); 
    for (; n<size; ++ n) { 
        ret = semctl (mid, n, GETZCNT); 
        if (ret < 0)
            err_sys ("semctl to get zcnt failed"); 

        printf ("       [%d].zcnt: %d\n", n, ret); 

        ret = semctl (mid, n, GETNCNT); 
        if (ret < 0)
            err_sys ("semctl to get ncnt failed"); 

        printf ("       [%d].ncnt: %d\n", n, ret); 

        ret = semctl (mid, n, GETPID); 
        if (ret < 0)
            err_sys ("semctl to get pid failed"); 

        printf ("       [%d].pid: %d\n", n, ret); 

#ifdef SEM_INIT
        sem.val = 10;
        ret = semctl (mid, n, SETVAL, sem); 
        if (ret < 0)
            err_sys ("semctl to set val failed"); 
#endif

        ret = semctl (mid, n, GETVAL); 
        if (ret < 0)
            err_sys ("semctl to get val failed"); 

        printf ("       [%d].val: %d\n\n", n, ret); 
    }

    if (with_perm)
        dump_perm (&mbuf.sem_perm); 
}

int main (int argc, char *argv[])
{
    int projid = -1, nsem = 1; 
    int put = 0;  // 0: get; 1: put
    if (argc > 1)
    {
        //printf ("Usage: xsisem <projid> <nsems> <mode (put|get)> [semid]\n"); 
        projid = atoi (argv[1]); 
    }

    if (argc > 2)
    {
        nsem = atoi (argv[2]); 
        printf ("num of sems: %d\n", nsem); 
    }

    if (argc > 3)
    {
        put = (strcasecmp (argv[3], "get") == 0 ? 0 : 1); 
        printf ("mode: %s\n", put == 0 ? "get" : "put"); 
    }

    int mid = 0; 
    int ret = 0, n = 0; 
    if (argc > 4)
    {
        mid = atol (argv[4]); 
        printf ("use mid %d\n", mid); 
    }
    else 
    {
        key_t key = IPC_PRIVATE; 
        if (projid >= 0)
            key = ftok ("./xsisem.c", projid); 

        int mode = 0666;  // 0; 
        int flag = IPC_CREAT; 
#ifdef USE_EXCL
        flag |= IPC_EXCL; 
#endif

        mid = semget (key, nsem, flag | mode); 
        if (mid < 0)
            err_sys ("semget for key 0x%08x failed", key); 

        printf ("create semaphore %d with key 0x%08x ok\n", mid, key); 
    }

    // after set mode bits in msgget, we don't need do this again.
    /// read access right is always needed as dumping queue, event for write process
    //set_mode (mid, 0, S_IRUSR | S_IRGRP | (put == 1 ? S_IWUSR | S_IWGRP : 0)); 
    dump_sem ("after open: ", mid, nsem, 1); 

    /*
    ssize_t res = 0; 
    size_t size = 0; 

    int flag = 0; 
    if (put == 0)
    {
#if USE_TYPE == 1
        n = MAX_QUEUE_SIZE; 
#elif USE_TYPE == 2
        n = -MAX_QUEUE_SIZE / 2; 
#else
        n = 0; 
#endif

        size = sizeof (buf.data); 
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
                break; 
            }
            else if (res == 0)
            {
                printf ("receive end message, type %ld, quit\n", buf.type); 
#ifdef DUMP_QUEUE
                dump_sem ("after recv: ", mid, nsem, 0); 
#endif
                break; 
            }

            buf.data[res] = 0;  // null terminated
            printf ("recv %d, type %ld: %s\n", res, buf.type, buf.data); 
#ifdef DUMP_QUEUE
            dump_sem ("after recv: ", mid, nsem, 0); 
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
            res = msgsnd (mid, &buf, strlen (buf.data), flag); 
            if (res < 0)
            {
                printf ("send msg failed, ret %d, errno %d\n", res, errno); 
                break; 
            }

            printf ("send %d, type %ld\n", res, buf.type); 
#ifdef DUMP_QUEUE
            dump_sem ("after send: ", mid, nsem, 0); 
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
            dump_sem ("after send: ", mid, nsem, 0); 
            sleep (1); 
#endif
        }

    }

    printf ("operate queue over\n"); 
    */

    switch (put)
    {
        case 0:
#ifdef READ_REMOVES
            goto DEFAULT; 
#else 
            break; 
#endif
        case 1:
#ifdef WRITE_REMOVES
            goto DEFAULT; 
#else
            break; 
#endif
DEFAULT:
        default:
        {
            ret = semctl (mid, 0, IPC_RMID, NULL); 
            if (ret < 0)
                err_sys ("semctl to remove semaphore failed"); 

            printf ("remove that semaphore\n"); 
            break; 
        }
    }

    printf ("exit\n"); 
    return 0; 
}
