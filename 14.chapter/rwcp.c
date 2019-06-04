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

#define BUF_SIZE 8192

int main (int argc, char *argv[])
{
    int fdin, fdout; 
    char buf[BUF_SIZE] = { 0 }; 
    if (argc != 3)
        handle_error ("usage: rwcp input output"); 

    fdin = open (argv[1], O_RDONLY); 
    if (fdin < 0)
        handle_error ("open input"); 

    fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
    if (fdout < 0)
        handle_error ("create output"); 

    int rd = 0, wr = 0; 
    while (1)
    {
        rd = read (fdin, buf, BUF_SIZE); 
        if (rd < 0)
            handle_error ("read input"); 
        else if (rd == 0)
            break; // reach end

        wr = write (fdout, buf, rd); 
        if (wr < rd)
            handle_error ("write output"); 
    }

    printf ("copy over\n"); 
    close (fdin); 
    close (fdout); 
    exit (0); 
}
