#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TEST_WRITE

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

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

    fd = open(argv[1], 
#ifdef TEST_WRITE
            O_RDWR);
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
            length = sb.st_size - offset;
                /* Can't display bytes past end of file */

    } else {    /* No length arg ==> display to end of file */
        length = sb.st_size - offset;
    }

    fprintf (stderr, "map %s (total %d) from %d (aligned %d) with length %d\n", argv[1], sb.st_size, offset, pa_offset, length); 
    addr = mmap(NULL, 
                length + offset - pa_offset, 
#ifdef TEST_WRITE
                PROT_READ | PROT_WRITE, 
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

#ifdef TEST_WRITE
    addr[offset-pa_offset] = 'b'; 
#endif

    s = write(STDOUT_FILENO, addr + offset - pa_offset, length);
    if (s != length) {
        if (s == -1)
            handle_error("write");

        fprintf(stderr, "partial write");
        exit(EXIT_FAILURE);
    }

#if 0
    // no need, when unmaped, all view will be flushed.
    ret = msync (addr, length + offset - pa_offset, MS_SYNC); 
    if (ret == -1)
        handle_error ("msync"); 
#endif

    ret = munmap (addr, length + offset - pa_offset); 
    if (ret == -1)
        handle_error ("munmap"); 

    close (fd); 

    exit(EXIT_SUCCESS);
} /* main */
