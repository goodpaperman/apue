#include "../apue.h" 
#include <fcntl.h> 
#include <sys/mman.h> 

int main (int argc, char *argv[])
{
    int fdin, fdout; 
    void *src, *dst; 
    struct stat statbuf; 
    if (argc != 3)
        err_quit ("usage: %s <fromfile> <tofile>", argv[0]); 

    fdin = open (argv[1], O_RDONLY); 
    if (fdin < 0)
        err_sys ("can't open %s for reading", argv[1]); 

    fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
    if (fdout < 0)
        err_sys ("can't create %s for writing", argv[2]); 

    if (fstat (fdin, &statbuf) < 0)
        err_sys ("fstat error"); 

    if (lseek (fdout, statbuf.st_size - 1, SEEK_SET) == -1)
        err_sys ("fstat error"); 

    if (write (fdout, "", 1) != 1)
        err_sys ("write error"); 

    src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0); 
    if (src == MAP_FAILED)
        err_sys ("mmap error for input"); 

    dst = mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0); 
    if (dst == MAP_FAILED)
        err_sys ("mmap error for output"); 

    memcpy (dst, src, statbuf.st_size); 
    exit (0); 
}
