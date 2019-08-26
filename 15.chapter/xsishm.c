#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL

//#define READ_REMOVEM
#define WRITE_REMOVEM
#define DUMP_SHM 

//#define MAX_SEM_SIZE 10

#include <sys/shm.h>

void set_mode (int mid, int mode)
{
    struct shmid_ds mbuf; 
    mbuf.shm_perm.uid = getpid (); 
    mbuf.shm_perm.gid = getpgid (getpid ()); 
    mbuf.shm_perm.mode = mode; 

    int ret = shmctl (mid, IPC_SET, &mbuf); 
    if (ret < 0)
        err_sys ("shmctl to set mode for %d failed", mid); 

    printf ("set shm mode ok\n"); 
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

/* return actually memory size when created */
int dump_shm (char const *prompt, int mid, int with_perm)
{
    struct shmid_ds mbuf; 
    int ret = shmctl (mid, IPC_STAT, &mbuf); 
    if (ret < 0)
        err_sys ("shmctl to stat shm failed"); 

    printf ("%s: \n"
            "   shm id: %d\n"
            "   segment size: %d\n"
            "   creator: %d\n"
            "   last shmop pid: %d\n"
            "   attach process: %d\n"
            "   last attach time: %d\n" 
            "   last detach time: %d\n" 
            "   last change time: %d\n", 
            prompt, 
            mid, 
            mbuf.shm_segsz, 
            mbuf.shm_cpid, 
            mbuf.shm_lpid, 
            mbuf.shm_nattch, 
            mbuf.shm_atime, 
            mbuf.shm_dtime, 
            mbuf.shm_ctime
            ); 

    int n = 0; 
    printf ("   \n"); 

    if (with_perm)
        dump_perm (&mbuf.shm_perm); 

    return mbuf.shm_segsz; 
}

int main (int argc, char *argv[])
{
    int projid = -1, size = 0; 
    int put = 0;  // 0: get; 1: put
    if (argc > 1)
    {
        //printf ("Usage: xsishm <projid> <size> <mode (put|get)> [shmid]\n"); 
        projid = atoi (argv[1]); 
    }

    if (argc > 2)
    {
        size = atoi (argv[2]); 
        printf ("size: %d\n", size); 
    }

    if (argc > 3)
    {
        put = (strcasecmp (argv[3], "get") == 0 ? 0 : 1); 
        printf ("mode: %s\n", put == 0 ? "get" : "put"); 
    }

    int mid = 0; 
    int ret = 0; 
    if (argc > 4)
        mid = atol (argv[4]); 

    if (mid > 0)
    {
        printf ("use mid %d directly\n", mid); 
    }
    else 
    {
        key_t key = IPC_PRIVATE; 
        if (projid >= 0)
            key = ftok ("./xsishm.c", projid); 

        int mode = 0666;  // 0; 
        int flag = IPC_CREAT; 
#ifdef USE_EXCL
        flag |= IPC_EXCL; 
#endif

        mid = shmget (key, size, flag | mode); 
        if (mid < 0)
            err_sys ("shmget for key 0x%08x failed", key); 

        printf ("create shared-memory %d with key 0x%08x ok\n", mid, key); 
    }

    // after set mode bits in msgget, we don't need do this again.
    /// read access right is always needed as dumping queue, event for write process
    //set_mode (mid, S_IRUSR | S_IRGRP | (put == 1 ? S_IWUSR | S_IWGRP : 0)); 
    size = dump_shm ("after open: ", mid, 1); 
    printf ("size updated to %d\n", size); 

//    int n = 0; 
//    int res = 0; 
//    struct sembuf *sb = malloc (sizeof (struct sembuf) * nsem) ; 
//    if (put == 0)
//    {
//        for (n=0; n<nsem; ++ n)
//        {
//#ifdef USE_ZERO
//            sb[n].sem_op = 0; 
//#else 
//            sb[n].sem_op = -op; 
//#endif
//            sb[n].sem_num = n; 
//            sb[n].sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
//#ifdef USE_UNDO
//            sb[n].sem_flg |= SEM_UNDO; 
//#endif
//#ifdef USE_NOWAIT
//            sb[n].sem_flg |= IPC_NOWAIT; 
//#endif
//        }
//
//        n = 0; 
//        while (1)
//        {
//            // read
//            res = semop (mid, sb, nsem); 
//            if (res < 0)
//            {
//                if (errno == EAGAIN)
//                {
//                    printf ("try 1 second later...\n"); 
//                    sleep (1); 
//                    continue; 
//                }
//
//                printf ("semop to request resource failed, ret %d, errno %d\n", res, errno); 
//                break; 
//            }
//
//            printf ("request %d resource %d, order %d\n", sb[n].sem_op, res, n++); 
//#ifdef DUMP_SHM
//            dump_shm ("after request: ", mid, 0); 
//            //sleep (1); 
//#endif 
//
//#ifdef USE_ZERO
//            // to avoid loop too many times when semaphore count down to 0.
//            usleep (100); 
//#endif
//        }
//    }
//    else 
//    {
//        int m; 
//        for (m=0; m<nsem; ++ m)
//        {
//            sb[m].sem_op = op; 
//            sb[m].sem_num = m; 
//            sb[m].sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
//#ifdef USE_UNDO
//            sb[m].sem_flg |= SEM_UNDO; 
//#endif
//#ifdef USE_NOWAIT
//            sb[m].sem_flg |= IPC_NOWAIT; 
//#endif
//        }
//
//        for (n=0; n<MAX_SEM_SIZE; ++ n)
//        {
//            for (m=0; m<nsem; ++ m)
//            {
//#ifdef USE_ZERO
//                // changed between -1 & 1.
//                sb[m].sem_op = n % 2 ? op : -op; 
//#endif
//            }
//
//            // write
//#ifdef DUMP_SHM
//            dump_shm ("before release: ", mid, 0); 
//#endif
//            res = semop (mid, sb, nsem); 
//            if (res < 0)
//            {
//                if (errno == EAGAIN)
//                {
//                    printf ("try 1 second later...\n"); 
//                    sleep (1); 
//                    continue; 
//                }
//
//                printf ("semop to release resource failed, ret %d, errno %d\n", res, errno); 
//                break; 
//            }
//
//            printf ("release %d return %d, order %d\n", sb[0].sem_op, res, n); 
//            sleep (1); 
//        }
//    }
//
//    free (sb); 
    printf ("operate shared-memory over\n"); 

    switch (put)
    {
        case 0:
#ifdef READ_REMOVEM
            goto DEFAULT; 
#else 
            break; 
#endif
        case 1:
#ifdef WRITE_REMOVEM
            goto DEFAULT; 
#else
            break; 
#endif
DEFAULT:
        default:
        {
            ret = shmctl (mid, IPC_RMID, NULL); 
            if (ret < 0)
                err_sys ("shmctl to remove shared-memory failed"); 

            printf ("remove that shared-memory\n"); 
            break; 
        }
    }

    printf ("exit\n"); 
    return 0; 
}
