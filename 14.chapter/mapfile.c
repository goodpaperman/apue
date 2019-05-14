#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> 

#define TEST_WRITE
#define TEST_SIGSEGV
//#define USE_MPROTECT

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#ifdef TEST_SIGSEGV
char ** g_addr = 0; 
int g_len = 0; 
int g_fd = 0; 
int g_off = 0; 

void sigsegv (int signo)
{
    int ret = 0; 
    printf ("caught signal %d\n", signo); 
#  ifdef USE_MPROTECT
    int ret = mprotect (*g_addr, g_len, PROT_READ | PROT_WRITE); 
    if (ret == 0)
        printf ("mprotect %p to rw ok\n", *g_addr); 
    else 
        handle_error("mprotect"); 
#  else 
    char *old_addr = *g_addr; 
    // works well when g_len > 0.
    //ret = munmap (old_addr, 1); 
    ret = munmap (old_addr, g_len); 
    if (ret < 0)
        handle_error ("munmap"); 

    *g_addr = mmap(old_addr,  /* do keep the addr not change !*/
                g_len, 
                PROT_READ | PROT_WRITE, 
                MAP_SHARED, 
                g_fd, 
                g_off);

    if (*g_addr == MAP_FAILED)
        handle_error("mmap");
    else 
    {
        printf ("remap %p to %p with length %d\n", old_addr, *g_addr, g_len); 
        if (old_addr != *g_addr)
            handle_error ("old addr not kept!"); 
    }
#  endif 
}
#endif

int
main(int argc, char *argv[])
{
    char *addr;
    int fd, ret;
    struct stat sb;
    off_t offset, pa_offset;
    size_t length;
    ssize_t s;

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "%s file offset [length]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

#ifdef TEST_SIGSEGV
    signal (SIGSEGV, sigsegv); 
#endif 

    fd = open(argv[1], 
#ifdef TEST_WRITE
            O_RDWR);
            // to test EPERM
            //O_RDONLY);
#else
            O_RDONLY);
#endif
    if (fd == -1)
        handle_error("open");

    if (fstat(fd, &sb) == -1)           /* To obtain file size */
        handle_error("fstat");

    offset = atoi(argv[2]);
    pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
        /* offset for mmap() must be page aligned */

    if (offset >= sb.st_size) {
        fprintf(stderr, "offset is past end of file\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 4) {
        length = atoi(argv[3]);
        if (offset + length > sb.st_size)
#if 1
            length = sb.st_size - offset;
                /* Can't display bytes past end of file */
#else 
            // can display but no back write indeed
            printf ("length %d pass end of file %d\n", length, sb.st_size); 
#endif

    } else {    /* No length arg ==> display to end of file */
        length = sb.st_size - offset;
    }

    int s_len = length + offset - pa_offset; 
    fprintf (stderr, "map %s (total %d) from %d (aligned %d) with length %d\n", argv[1], sb.st_size, offset, pa_offset, length); 
    addr = mmap(NULL, 
                s_len, 
#ifdef TEST_WRITE
#  ifdef TEST_SIGSEGV
                PROT_READ, 
#  else
                PROT_READ | PROT_WRITE, 
#  endif
                MAP_SHARED, 
                //MAP_PRIVATE, 
#else
                PROT_READ, 
                MAP_PRIVATE, 
#endif 
                fd, 
                pa_offset);

    if (addr == MAP_FAILED)
        handle_error("mmap");

#ifdef TEST_SIGSEGV
    g_addr = &addr; 
    g_len = s_len; 
    g_fd = fd; 
    g_off = pa_offset; 
#endif

#ifdef TEST_WRITE
    // write backend to test map pass end of file and write is nonsense
    addr[s_len - 1] = 'b';  
#endif

    s = write(STDOUT_FILENO, addr + offset - pa_offset, length);
    if (s != length) {
        if (s == -1)
            handle_error("write");

        fprintf(stderr, "partial write");
        exit(EXIT_FAILURE);
    }

#if 1
    // need, when unmaped, view will not be flushed.
    ret = msync (addr, s_len, MS_SYNC); 
    // fatal error, parameter addr must block aligned!!
    //ret = msync (addr + offset - pa_offset, length, MS_SYNC); 
    if (ret == -1)
        handle_error ("msync"); 
#endif

    close (fd); 
    
    // to see if we can access maps after close file
    printf ("after alter %d: %c\n", s_len - 1, addr[s_len - 1]); 
    ret = munmap (addr, s_len); 
    if (ret == -1)
        handle_error ("munmap"); 

    exit(EXIT_SUCCESS);
} /* main */
