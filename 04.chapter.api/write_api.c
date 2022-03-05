#include "../apue.h"

int main (int argc, char *argv[])
{
    if (argc < 5)
        err_quit ("Usage: write_api path offset length char\n", 1); 

    char *buf = NULL; 
    int fd = open (argv[1], O_WRONLY);
    if (fd < 0)
        err_sys ("open file %s failed", argv[1]); 
    
    do
    {
        int off = atoi(argv[2]); 
        int len = atoi(argv[3]); 
        char ch = argv[4][0]; 
        buf = (char *)malloc(len); 
        memset (buf, ch, len); 
        if (buf == NULL) {
            printf ("alloc buffer with len %d failed\n", len); 
            break; 
        }

        if (lseek (fd, off, SEEK_SET) != off) {
            printf ("seek to %d failed\n", off); 
            break; 
        }

        if (write (fd, buf, len) != len) {
            printf("write %d at %d failed\n", len, off); 
            break; 
        }

        printf ("write %d '%c' ok\n", len, ch); 
    } while (0);

    free (buf); 
    close (fd); 
    return 0; 
}


