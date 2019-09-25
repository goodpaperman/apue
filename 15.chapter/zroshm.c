#include "../apue.h" 
#include <fcntl.h> 
#include <sys/mman.h> 
#include <errno.h> 

#define USE_SHM

#ifndef USE_SHM
#  define USE_ANON
//#  define USE_NULL
//#  define USE_PRIVATE
#else 
#  include <sys/shm.h> 
#endif

#define NLOOPS 1000
#define SIZE sizeof(long)

static int update (long *ptr)
{
    return ((*ptr) ++); 
}

int main (void)
{
    int ret = 0; 
    void *area = 0; 
#ifdef USE_SHM
    int mode = 0666;  // 0; 
    int flag = IPC_CREAT; 
#  ifdef USE_EXCL
    flag |= IPC_EXCL; 
#  endif

    int mid = shmget (IPC_PRIVATE, SIZE, flag | mode); 
    if (mid < 0)
        err_sys ("shmget for size %d failed", SIZE); 

    printf ("create shared-memory %d with size %d ok\n", mid, SIZE); 

#  ifdef USE_RND
    flag = SHM_RND; 
    area = 0x8004001; 
    printf ("try addr 0x%08x\n", area); 
#  endif

#  ifdef USE_RDONLY
    flag |= SHM_RDONLY; 
    printf ("try open map readonly\n"); 
#  endif

   area = shmat (mid, area, flag); 
   if (area == -1)
       err_sys ("shmat failed, errno %d", errno); 

   printf ("attach shared-memory at %p\n", area); 
#else
# ifdef USE_ANON
    int fd = -1; 
#  else 
#    ifdef USE_NULL
    int fd = open ("/dev/null", O_RDWR); 
#    else
    int fd = open ("/dev/zero", O_RDWR); 
#    endif
    if (fd < 0)
        err_sys ("open error"); 

    printf ("open /dev/zero ok\n"); 
#  endif

    int flag = MAP_SHARED; 
#  ifdef USE_PRIVATE
    flag = MAP_PRIVATE; 
#  endif

#  ifdef USE_ANON
    flag |= MAP_ANONYMOUS; 
#  endif

    area = mmap (0, SIZE, PROT_READ | PROT_WRITE, flag, fd, 0); 
    if (area == MAP_FAILED)
        err_sys ("mmap error"); 

    printf ("map long from that file ok\n"); 
    close (fd); 
#endif

    SYNC_INIT(); 

    int i=0, counter=0; 
    pid_t pid = fork (); 
    if (pid < 0)
        err_sys ("fork error"); 
    else if (pid > 0)
    {
        // parent
        for (i=0; i<NLOOPS; i+=2)
        {
            counter = update ((long *)area); 
            if (counter != i)
                err_quit ("parent: expected %d, got %d", i, counter); 
            else 
                printf ("parent increase to %d based %d\n", i+1, counter); 

            SYNC_TELL(pid, 1); 
            SYNC_WAIT(); 
        }

        printf ("parent exit\n"); 
    }
    else 
    {
        for (i=1; i<NLOOPS+1; i+=2)
        {
            SYNC_WAIT(); 
            counter = update ((long *)area); 
            if (counter != i)
                err_quit ("child: expected %d, got %d", i, counter); 
            else 
                printf ("child increase to %d based %d\n", i+1, counter); 

            SYNC_TELL(getppid (), 0);
        }

        printf ("child exit\n"); 
    }

#ifdef USE_SHM
    shmdt (area); 
    if (pid > 0)
    {
        ret = shmctl (mid, IPC_RMID, NULL); 
        if (ret < 0)
            err_sys ("shmctl to remove shared-memory failed"); 

        printf ("remove that shared-memory\n"); 
    }
#else 
    munmap (area, SIZE); 
#endif 

    return 0; 
}
