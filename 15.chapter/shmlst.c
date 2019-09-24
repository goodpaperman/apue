#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define DEBUG
//#define USE_EXCL
//#define USE_RND
//#define DUMP_SHM 

#define MAX_SHM_SIZE 1024
#define MAX_DATA_SIZE (32-4-4)
#define MAX_DATA_ITEM 1000

#include <sys/shm.h>
#include <sys/sem.h>

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

union semun
{
  int val;				//<= value for SETVAL
  struct semid_ds *buf; //		<= buffer for IPC_STAT & IPC_SET
  unsigned short int *array;// 		<= array for GETALL & SETALL
  struct seminfo *__buf;    //		<= buffer for IPC_INFO
};

// return real nsems
int dump_sem (char const *prompt, int mid, int with_perm)
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

        ret = semctl (mid, n, GETVAL); 
        if (ret < 0)
            err_sys ("semctl to get val failed"); 

        printf ("       [%d].val: %d\n", n, ret); 
        printf ("\n"); 
    }

    if (with_perm)
        dump_perm (&mbuf.sem_perm); 

    return mbuf.sem_nsems; 
}

struct slist
{
    int offset; 
    int inuse; 
    char data[MAX_DATA_SIZE]; 

    // not support function in struct for C
    //struct slist *get_next(void *base) { return (struct slist *)(base + offset); }
    //void set_next(void *base, void *ptr) { offset=(ptr)-(base); }
#define get_next(base) offset + (void *)base
#define set_next(base, ptr) offset=((void *)ptr)-((void *)base)
}; 

void init_node (void *base)
{
    struct slist *head = (struct slist *)base; 
    head->inuse = 1;
    head->set_next (base, base); 
    printf ("init_node ok\n"); 
}

int add_node (void *base, int size, int n)
{
    struct slist *head = (struct slist *)base; 
    struct slist *ptr = head->get_next(base), *prev = head; 
#ifdef DEBUG
    printf ("add_node_1: head %p, next %p\n", head, ptr); 
#endif
    while (ptr != head)
    {
        prev = ptr; 
        ptr = ptr->get_next(base); 
#ifdef DEBUG
        printf ("add_node_2: prev %p, next %p\n", prev, ptr); 
#endif
    }

    // find out last node at 'prev'
    struct slist *curr = prev + 1; 
    while (curr->inuse && curr + 1 < base + size)
    {
        curr++; 
#ifdef DEBUG
        printf ("add_node_3: find %p\n", curr); 
#endif
    }

    if (curr+1 > base + size)
    {
        // overflow ? run back
        printf ("no free node in after section, try before...\n"); 
        curr = head + 1; 
        while (curr->inuse && curr < prev)
        {
            curr ++; 
#ifdef DEBUG
            printf ("add_node_4: find %p\n", curr); 
#endif
        }

        if (curr >= prev)
        {
            // no more place
            printf ("no memory !\n"); 
            return 0; 
        }
    }


    curr->inuse = 1; 
    sprintf (curr->data, "this is %d", n); 
    curr->set_next (base, head); 
#ifdef DEBUG
    printf ("add_node_5: curr %p, next %p\n", curr, curr->get_next(base)); 
#endif

    prev->set_next (base, curr); 
#ifdef DEBUG
    printf ("add_node_6: prev %p, next %p\n", prev, prev->get_next(base)); 
#endif

    printf ("add node %d ok\n", n); 
    return 1; 
}

int remove_node (void *base, char **data)
{
    struct slist *head = (struct slist *)base; 
#ifdef DEBUG
    printf ("remove_node_1: head %p, next %p\n", head, head->get_next(base)); 
#endif
    if (head->offset == 0)
        return 0; // no more

    struct slist *ptr = head->get_next(base); 
    *data = ptr->data; 
    ptr->inuse = 0; 

    struct slist *next = ptr->get_next(base); 
    head->set_next(base, next); 
#ifdef DEBUG
    printf ("remove_node_2: head %p, next %p\n", head, head->get_next(base)); 
#endif

    printf ("remove node %s ok\n", ptr->data); 
    return 1; 
}

int main (int argc, char *argv[])
{
    int projid = 42, size = MAX_SHM_SIZE; 
    int put = 0;  // 0: get; 1: put
    if (argc > 1)
    {
        //printf ("Usage: xsishm <mode (put|get)>\n"); 
        put = (strcasecmp (argv[1], "get") == 0 ? 0 : 1); 
        printf ("mode: %s\n", put == 0 ? "get" : "put"); 
    }

    int mid = 0, sid = 0, nsem = 0; 
    int ret = 0; 
    key_t key = IPC_PRIVATE; 
    key = ftok ("./shmlst.c", projid); 

    int mode = 0666;  // 0; 
    int flag = IPC_CREAT; 
#ifdef USE_EXCL
    flag |= IPC_EXCL; 
#endif

    mid = shmget (key, size, flag | mode); 
    if (mid < 0)
        err_sys ("shmget for key 0x%08x failed", key); 

    printf ("create shared-memory %d with key 0x%08x ok\n", mid, key); 

    size = dump_shm ("after open: ", mid, 1); 
    printf ("size updated to %d\n", size); 

    sid = semget (key, 1, flag | mode); 
    if (sid < 0)
        err_sys ("semget for key 0x%08x failed", key); 

    printf ("create semaphore %d with key 0x%08x ok\n", sid, key); 

    struct sembuf sb; 
    if (put == 1)
    {
        // set init value
        union semun sem; 
        sem.val = 0;
        ret = semctl (sid, 0, SETVAL, sem); 
        if (ret < 0)
            err_sys ("semctl to init resource failed, ret %d, errno %d\n", ret, errno); 
        else 
            printf ("set sempahore init value to %d\n", sem.val); 
    }

    nsem = dump_sem ("after open: ", sid, 1); 
    printf ("update nsems to %d\n", nsem); 

    int n=0, val = 0; 
    void* addr = 0; 
    flag = 0; 
#ifdef USE_RND
    flag |= SHM_RND; 
    addr = 0x8004001; 
    printf ("try addr 0x%08x\n", addr); 
#endif

    if (put == 0)
    {
        // although for read, we need change the memory content.
        //flag |= SHM_RDONLY; 
        addr = shmat (mid, addr, flag); 
        if (addr == -1)
            err_sys ("shmat failed, errno %d", errno); 

        printf ("attach shared-memory at %p\n", addr); 
#ifdef DUMP_SHM
        dump_shm ("after attach", mid, 0); 
#endif

        char *ptr = NULL; 
        while (1)
        {
            // if any resouce available ?
            sb.sem_op = -1; 
            sb.sem_num = 0; 
            sb.sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
            ret = semop (sid, &sb, 1); 
            if (ret < 0)
                err_sys ("semop to require resource failed, ret %d, errno %d\n", ret, errno); 
            else 
                printf ("get 1 resource from semaphore\n"); 

            if (!remove_node (addr, &ptr))
            {
                printf ("reach end\n"); 
                break; 
            }

            printf ("got msg: %s\n", ptr); 
            // no sleep for read any more, we will block at semaphore when no data..
            //sleep (1); 
            usleep (100000); 
        }

        shmdt (addr); 
#ifdef DUMP_SHM
        dump_shm ("after detach", mid, 0); 
#endif

    }
    else
    {
        addr = shmat (mid, addr, flag); 
        if (addr == -1)
            err_sys ("shmat failed, errno %d", errno); 

        printf ("attach shared-memory at %p\n", addr); 
#ifdef DUMP_SHM
        dump_shm ("after attach", mid, 0); 
#endif

        // no need to ..
        //init_node (addr); 
        for (n=0; n<MAX_DATA_ITEM; ++ n)
        {
            if (add_node (addr, size, n))
                printf ("create msg %d\n", n); 
            else 
            {
                -- n; 
                printf ("wait a while...\n"); 
                usleep (100000); 
                //sleep (1); 
                continue; 
            }

            sb.sem_op = 1; 
            sb.sem_num = 0; 
            sb.sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
            ret = semop (sid, &sb, 1); 
            if (ret < 0)
                err_sys ("semop to release resource failed, ret %d, errno %d\n", ret, errno); 
            else 
                printf ("release 1 resource from semaphore\n"); 

            // no sleep for write
            //usleep (100000); 
        }

        shmdt (addr); 
#ifdef DUMP_SHM
        dump_shm ("after detach", mid, 0); 
#endif
    }

    if (put == 0)
    {
        ret = shmctl (mid, IPC_RMID, NULL); 
        if (ret < 0)
            err_sys ("shmctl to remove shared-memory failed"); 

        printf ("remove that shared-memory\n"); 

        ret = semctl (sid, 0, IPC_RMID, NULL); 
        if (ret < 0)
            err_sys ("semctl to remove semaphore failed"); 

        printf ("remove that semaphore\n"); 
    }
    else 
    {
        // notify read there is no data any more !
        sb.sem_op = 1; 
        sb.sem_num = 0; 
        sb.sem_flg = 0;  // IPC_NOWAIT, SEM_UNDO
        ret = semop (sid, &sb, 1); 
        if (ret < 0)
            err_sys ("semop to release resource failed, ret %d, errno %d\n", ret, errno); 
        else 
            printf ("notify semaphore to ensure reader exit..\n"); 
    }

    printf ("exit\n"); 
    return 0; 
}
