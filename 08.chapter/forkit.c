#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 

// for virtual2physical
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

unsigned long virtual2physical(void* ptr)
{
    unsigned long vaddr = (unsigned long)ptr; 
    int pageSize = getpagesize();
    unsigned long v_pageIndex = vaddr / pageSize;
    unsigned long v_offset = v_pageIndex * sizeof(uint64_t);
    unsigned long page_offset = vaddr % pageSize;
    uint64_t item = 0;

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if(fd == -1)
    {
        printf("open /proc/self/pagemap error\n");
        return NULL;
    }

    if(lseek(fd, v_offset, SEEK_SET) == -1)
    {
        printf("sleek error\n");
        return NULL; 
    }

    if(read(fd, &item, sizeof(uint64_t)) != sizeof(uint64_t))
    {
        printf("read item error\n");
        return NULL;
    }

    if((((uint64_t)1 << 63) & item) == 0)
    {
        printf("page present is 0\n");
        return NULL;
    }

    uint64_t phy_pageIndex = (((uint64_t)1 << 55) - 1) & item;
    return (unsigned long)((phy_pageIndex * pageSize) + page_offset);
}

int g_count = 1; 
int main()
{
    int v_count = 42; 
    static int s_count = 1024; 
    int* h_count = (int*)malloc(sizeof(int)); 
    *h_count = 36; 
    printf ("%d: global ptr 0x%x:0x%x, local ptr 0x%x:0x%x, static ptr 0x%x:0x%x, heap ptr 0x%x:0x%x\n", getpid(), 
            &g_count, virtual2physical(&g_count), 
            &v_count, virtual2physical(&v_count), 
            &s_count, virtual2physical(&s_count), 
            h_count, virtual2physical(h_count)); 

#ifdef USE_VFORK
    int pid = vfork(); 
#else
    int pid = fork(); 
#endif
    if (pid < 0)
    {
        // error
        exit(1); 
    }
    else if (pid == 0)
    {
        // child
        printf ("%d spawn from %d\n", getpid(), getppid()); 
#if 1
        g_count ++; 
        v_count ++; 
        s_count ++; 
        (*h_count) ++; 
#endif

#ifdef USE_VFORK
        printf ("%d: global %d, local %d, static %d, heap %d\n", getpid(), g_count, v_count, s_count, *h_count); 
        printf ("%d: global ptr 0x%x:0x%x, local ptr 0x%x:0x%x, static ptr 0x%x:0x%x, heap ptr 0x%x:0x%x\n", getpid(), 
                &g_count, virtual2physical(&g_count), 
                &v_count, virtual2physical(&v_count), 
                &s_count, virtual2physical(&s_count), 
                h_count, virtual2physical(h_count)); 

#  if 1
        _exit(0);
#  else
        exit(0); 
#  endif
#endif
    }
    else 
    {
        // parent
#ifndef USE_VFORK
        sleep (1); 
#endif
        printf ("%d create %d\n", getpid(), pid); 
    }

    printf ("%d: global %d, local %d, static %d, heap %d\n", getpid(), g_count, v_count, s_count, *h_count); 
    printf ("%d: global ptr 0x%x:0x%x, local ptr 0x%x:0x%x, static ptr 0x%x:0x%x, heap ptr 0x%x:0x%x\n", getpid(), 
            &g_count, virtual2physical(&g_count), 
            &v_count, virtual2physical(&v_count), 
            &s_count, virtual2physical(&s_count), 
            h_count, virtual2physical(h_count)); 

    return 0; 
}
