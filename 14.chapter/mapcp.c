//#include "../apue.h" 
#include <fcntl.h> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

//#define TEST_SIGBUS
#define USE_TRUNCATE

#ifdef TEST_SIGBUS
int g_fdout = 0; 
int g_len = 0; 

void sigbus (int signo)
{
    printf ("caught a signal %d\n", signo); 
    if (ftruncate(g_fdout, g_len) == -1)
        handle_error("ftruncate"); 
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
        handle_error ("wrong usage"); 

#ifdef TEST_SIGBUS
    signal (SIGBUS, sigbus); 
#endif

    fdin = open (argv[1], O_RDONLY); 
    if (fdin < 0)
        handle_error ("open input"); 

    fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
    if (fdout < 0)
        handle_error ("create output"); 

    if (fstat (fdin, &statbuf) < 0)
        handle_error ("fstat"); 

#ifdef TEST_SIGBUS
    g_fdout = fdout; 
    g_len = statbuf.st_size; 
#else
#  ifdef USE_TRUNCATE
    // not portable than lseek + write
    // but still very portable
    if (ftruncate(fdout, statbuf.st_size) == -1)
        handle_error ("ftruncate"); 
#  else
    if (lseek (fdout, statbuf.st_size - 1, SEEK_SET) == -1)
        handle_error ("lseek"); 

    if (write (fdout, "", 1) != 1)
        handle_error ("write"); 
#  endif
#endif

    src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0); 
    if (src == MAP_FAILED)
        handle_error ("mmap input"); 

    dst = mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0); 
    if (dst == MAP_FAILED)
        handle_error ("mmap output"); 

    memcpy (dst, src, statbuf.st_size); 
    printf ("copy over\n"); 
    exit (0); 
}
