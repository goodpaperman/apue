#include "../apue.h" 
#include <fcntl.h> 
#include <sys/mman.h> 
#include <errno.h> 

//#define TEST_SIGBUS
#define USE_TRUNCATE

#ifdef TEST_SIGBUS
int g_fdout = 0; 
int g_len = 0; 

void sigbus (int signo)
{
    printf ("caught a signal %d\n", signo); 
    if (ftruncate(g_fdout, g_len) == -1)
        err_sys("ftruncate error"); 
    else 
        printf ("truncate file %d to new length %d\n", g_fdout, g_len); 
}
#endif 

int main (int argc, char *argv[])
{
    int fdin, fdout; 
    void *src, *dst; 
    struct stat statbuf; 
    if (argc != 3)
        err_quit ("usage: %s <fromfile> <tofile>", argv[0]); 

#ifdef TEST_SIGBUS
    signal (SIGBUS, sigbus); 
#endif

    fdin = open (argv[1], O_RDONLY); 
    if (fdin < 0)
        err_sys ("can't open %s for reading", argv[1]); 

    fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
    if (fdout < 0)
        err_sys ("can't create %s for writing", argv[2]); 

    if (fstat (fdin, &statbuf) < 0)
        err_sys ("fstat error"); 

#ifdef TEST_SIGBUS
    g_fdout = fdout; 
    g_len = statbuf.st_size; 
#else
#  ifdef USE_TRUNCATE
    // not portable than lseek + write
    // but still very portable
    if (ftruncate(fdout, statbuf.st_size) == -1)
        err_sys("ftruncate error"); 
#  else
    if (lseek (fdout, statbuf.st_size - 1, SEEK_SET) == -1)
        err_sys ("lseek error"); 

    if (write (fdout, "", 1) != 1)
        err_sys ("write error"); 
#  endif
#endif

    src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0); 
    if (src == MAP_FAILED)
        err_sys ("mmap error for input"); 

    dst = mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0); 
    if (dst == MAP_FAILED)
        err_sys ("mmap error for output"); 

    memcpy (dst, src, statbuf.st_size); 
    exit (0); 
}
