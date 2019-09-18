#include "../apue.h" 
#include <strings.h> 
#include <errno.h> 

//#define USE_EXCL
//#define USE_RND
//#define DUMP_SHM 
#define MAX_DATA_SIZE 1024
#define MAX_DATA_ITEM 10

#include <sys/shm.h>

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

struct slist
{
    int offset; 
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
    head->set_next (base, base); 
    printf ("init_node ok\n"); 
}

int add_node (void *base, int n)
{
    struct slist *head = (struct slist *)base; 
    struct slist *ptr = head->get_next(base), *prev = head; 
    while (ptr != head)
    {
        prev = ptr; 
        ptr = ptr->get_next(base); 
    }

    // find out last node at 'prev'
    struct slist *curr = prev + 1; 
    sprintf (curr->data, "this is %d", n); 
    curr->set_next (base, head); 

    prev->set_next (base, curr); 
    printf ("add node %d ok\n", n); 
    return 1; 
}

int remove_node (void *base, char **data)
{
    struct slist *head = (struct slist *)base; 
    if (head->offset == 0)
        return 0; // no more

    struct slist *ptr = head->get_next(base); 
    *data = ptr->data; 

    struct slist *next = ptr->get_next(base); 
    head->set_next(base, next); 
    printf ("remove node %s ok\n", ptr->data); 
    return 1; 
}

int main (int argc, char *argv[])
{
    int projid = 42, size = 1024; 
    int put = 0;  // 0: get; 1: put
    if (argc > 1)
    {
        //printf ("Usage: xsishm <mode (put|get)> [shmid]\n"); 
        put = (strcasecmp (argv[1], "get") == 0 ? 0 : 1); 
        printf ("mode: %s\n", put == 0 ? "get" : "put"); 
    }

    int mid = 0; 
    int ret = 0; 
    if (argc > 2)
        mid = atol (argv[4]); 

    if (mid > 0)
    {
        printf ("use mid %d directly\n", mid); 
    }
    else 
    {
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
    }

    size = dump_shm ("after open: ", mid, 1); 
    printf ("size updated to %d\n", size); 

    int n=0, val = 0; 
    void* addr = 0; 
    int flag = 0; 
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
            if (!remove_node (addr, &ptr))
            {
                printf ("reach end\n"); 
                break; 
            }

            printf ("got msg: %s\n", ptr); 
            sleep (1); 
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
            add_node (addr, n); 
            printf ("create msg %d\n", n); 
            sleep (1); 
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
    }

    printf ("exit\n"); 
    return 0; 
}
