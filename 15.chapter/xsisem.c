#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL
#define SEM_INIT
//#define USE_ARRAY

#define READ_REMOVES
#define WRITE_REMOVES
//#define DUMP_SEM
#define MAX_SEM_SIZE 10

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

void dump_sem (char const *prompt, int mid, int with_perm)
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
#ifdef USE_ARRAY
    unsigned short *arr = calloc (sizeof (unsigned short), mbuf.sem_nsems); 
    if (arr == NULL)
        err_sys ("malloc array failed\n"); 

    sem.array = arr; 
#  ifdef SEM_INIT
    for (n=0; n<mbuf.sem_nsems; ++ n) {
        arr[n] = 10;
    }

    ret = semctl (mid, 0, SETALL, sem); 
    if (ret < 0)
        err_sys ("semctl to set all val failed"); 

#  endif

    ret = semctl (mid, 0, GETALL, sem); 
    if (ret < 0)
        err_sys ("semctl to get all val failed"); 

    for (n=0; n<mbuf.sem_nsems; ++ n) { 
        printf ("       [%d].val: %d\n", n, sem.array[n]); 
    }
#else
    for (; n<mbuf.sem_nsems; ++ n) { 
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

#  ifdef SEM_INIT
        sem.val = 10;
        ret = semctl (mid, n, SETVAL, sem); 
        if (ret < 0)
            err_sys ("semctl to set val failed"); 
#  endif

        ret = semctl (mid, n, GETVAL); 
        if (ret < 0)
            err_sys ("semctl to get val failed"); 

        printf ("       [%d].val: %d\n", n, ret); 
        printf ("\n"); 
    }
#endif

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
    int ret = 0; 
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
    dump_sem ("after open: ", mid, 1); 


    int n = 0; 
    int res = 0; 
    struct sembuf sb; 
    sb.sem_num = 0; 
    sb.sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
    if (put == 0)
    {
        sb.sem_op = -1; 
        while (1)
        {
            // read
            res = semop (mid, &sb, 1); 
            if (res < 0)
            {
                printf ("semop to request resource failed, ret %d, errno %d\n", res, errno); 
                break; 
            }

            printf ("request a resource %d, order %d\n", res, n++); 
#ifdef DUMP_SEM
            dump_sem ("after request: ", mid, 0); 
            //sleep (1); 
#endif 
        }
    }
    else 
    {
        sb.sem_op = 1; 
        for (n=0; n<MAX_SEM_SIZE; ++ n)
        {
            // write
#ifdef DUMP_SEM
            dump_sem ("before release: ", mid, 0); 
#endif
            res = semop (mid, &sb, 1); 
            if (res < 0)
            {
                printf ("semop to release resource failed, ret %d, errno %d\n", res, errno); 
                break; 
            }

            printf ("release %d, order %d\n", res, n); 
            sleep (1); 
        }
    }

    printf ("operate semaphore over\n"); 
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
