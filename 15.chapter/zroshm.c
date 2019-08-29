#include "../apue.h" 
#include <fcntl.h> 
#include <sys/mman.h> 

#define USE_ANON
//#define USE_NULL
//#define USE_PRIVATE

#define NLOOPS 1000
#define SIZE sizeof(long)

static int update (long *ptr)
{
    return ((*ptr) ++); 
}

int main (void)
{
#ifdef USE_ANON
    int fd = -1; 
#else 
#  ifdef USE_NULL
    int fd = open ("/dev/null", O_RDWR); 
#  else
    int fd = open ("/dev/zero", O_RDWR); 
#  endif
    if (fd < 0)
        err_sys ("open error"); 

    printf ("open /dev/zero ok\n"); 
#endif

    int flag = MAP_SHARED; 
#ifdef USE_PRIVATE
    flag = MAP_PRIVATE; 
#endif

#ifdef USE_ANON
    flag |= MAP_ANONYMOUS; 
#endif

    void *area = mmap (0, SIZE, PROT_READ | PROT_WRITE, flag, fd, 0); 
    if (area == MAP_FAILED)
        err_sys ("mmap error"); 

    printf ("map long from that file ok\n"); 
    close (fd); 
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

    return 0; 
}
