#include "../apue.h" 
#include <unistd.h> 
#include <sys/stat.h>

#define SHUTDOWN_PARENT_READ
//#define SHUTDOWN_PARENT_WRITE

int main (int argc, char *argv[])
{
    int ret = 0; 
    char buf[2] = { 0 }; 
    int fd[2] = { 0 }; 
    if (pipe(fd) < 0) 
        err_sys ("pipe"); 

    printf ("parent %lu start\n", getpid ()); 

    struct stat fs = { 0 }; 
    if (fstat (fd[0], &fs) < 0)
        err_sys ("fstat"); 

    printf ("fd[0] st_size: %d\n", fs.st_size); 
    printf ("S_ISFIFO(fd[0]): %d\n", S_ISFIFO(fs.st_mode)); 
    if (fstat (fd[1], &fs) < 0)
        err_sys ("fstat"); 

    printf ("fd[1] st_size: %d\n", fs.st_size); 
    printf ("S_ISFIFO(fd[1]): %d\n", S_ISFIFO(fs.st_mode)); 
    pid_t pid = fork (); 
    if (pid == 0) { 
        // child
#ifdef SHUTDOWN_PARENT_READ
        close (fd[1]); 
        ret = read (fd[0], buf, 1); 
        printf ("child read %c\n", buf[0]); 
        ret = read (fd[0], buf, 1); 
        printf ("child read %c\n", buf[0]); 
#elif defined (SHUTDOWN_PARENT_WRITE)
        close (fd[0]); 
        ret = write (fd[1], "aA", 2); 
        printf ("child write %d with a\n", ret); 
#else
        printf ("child start\n"); 
        ret = write (fd[1], "aA", 2); 
        printf ("child write %d with a\n", ret); 
        ret = read (fd[0], buf, 1); 
        printf ("child read %c\n", buf[0]); 
        sleep (2); 
        ret = read (fd[0], buf, 1); 
        printf ("child read %c\n", buf[0]); 
#endif
        //return 0; 
    }
    else if (pid > 0) { 
        // parent
        printf ("start child %lu\n", pid); 
#ifdef SHUTDOWN_PARENT_READ
        close (fd[0]); 
        ret = write (fd[1], "bB", 2); 
        printf ("parent write %d with b\n", ret); 
#  if 0
        sleep (2); 
        ret = write (fd[1], "bB", 2); 
        printf ("parent write %d with b\n", ret); 
#  endif
#elif defined (SHUTDOWN_PARENT_WRITE)
        close (fd[1]); 
        ret = read (fd[0], buf, 1); 
        printf ("parent read %c\n", buf[0]); 
        sleep (1); 
        ret = read (fd[0], buf, 1); 
        printf ("parent read %c\n", buf[0]); 
#else
        ret = write (fd[1], "bB", 2); 
        printf ("parent write %d with b\n", ret); 
        sleep (1); 
        ret = read (fd[0], buf, 1); 
        printf ("parent read %c\n", buf[0]); 
        sleep (1); 
        ret = read (fd[0], buf, 1); 
        printf ("parent read %c\n", buf[0]); 
#endif
    }
    else 
        err_sys ("fork"); 


    sleep (20); 
    return 0; 
}
