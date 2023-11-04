#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 

// for mem_addr
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

unsigned long mem_addr(unsigned long vaddr)
{
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
    printf ("%d: global ptr %x:%x, local ptr %x:%x, static ptr %x:%x\n", getpid(), &g_count, mem_addr(&g_count), &v_count, mem_addr(&v_count), &s_count, mem_addr(&s_count)); 
    int pid = fork(); 
    if (pid < 0)
    {
        // error
        exit(1); 
    }
    else if (pid == 0)
    {
        // child
        printf ("%d spawn from %d\n", getpid(), getppid()); 
        g_count ++; 
        v_count ++; 
        s_count ++; 
    }
    else 
    {
        // parent
        sleep (1); 
        printf ("%d create %d\n", getpid(), pid); 
    }

    printf ("%d: global %d, local %d, static %d\n", getpid(), g_count, v_count, s_count); 
    printf ("%d: global ptr %x:%x, local ptr %x:%x, static ptr %x:%x\n", getpid(), &g_count, mem_addr(&g_count), &v_count, mem_addr(&v_count), &s_count, mem_addr(&s_count)); 
    return 0; 
}
