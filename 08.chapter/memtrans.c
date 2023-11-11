#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
 
void test_ptr(void *ptr, char const* prompt)
{
   uint64_t addr = virtual2physical(ptr); 
   printf ("%s: virtual: 0x%x, physical: 0x%x\n", prompt, ptr, addr); 
}

int g_val1=0; 
int g_val2=1; 
 
int main(void) {
    test_ptr(&g_val1, "global1"); 
    test_ptr(&g_val2, "global2"); 

    int l_val3=2; 
    int l_val4=3; 
    test_ptr(&l_val3, "local1"); 
    test_ptr(&l_val4, "local2"); 

    static int s_val5=4; 
    static int s_val6=5; 
    test_ptr(&s_val5, "static1"); 
    test_ptr(&s_val6, "static2"); 

    int *h_val7=(int*)malloc(sizeof(int)); 
    int *h_val8=(int*)malloc(sizeof(int)); 
    test_ptr(h_val7, "heap1"); 
    test_ptr(h_val8, "heap2"); 
    free(h_val7); 
    free(h_val8); 
    return 0; 
}; 

