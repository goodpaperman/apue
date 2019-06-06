#include "../apue.h" 
#include <unistd.h> 
#include <sys/stat.h>

// to test is the PIPE_BUF atom indeed ?
//#define PIPEBUF_SLICE

#ifdef PIPEBUF_SLICE
#  define N 20
#  define M 100
#  define S 16
#  define P S 
#else 
#  define N 10
#  define M 10
#  define P 3
#endif 

int main (int argc, char *argv[])
{
    int ret = 0, i = 0, j = 0; 
    int fd[2] = { 0 }; 
    if (pipe(fd) < 0) 
        err_sys ("pipe"); 

    printf ("parent %lu start\n", getpid ()); 
    int pipe_buf = pathconf (".", _PC_PIPE_BUF); 

#ifdef PIPEBUF_SLICE
    pipe_buf = pipe_buf / S; 
#endif 

    printf ("PIPE_BUF = %d\n", pipe_buf); 
    char *buf = (char *) malloc(pipe_buf * P); 

    for (i=0; i<N; ++ i) { 
        pid_t pid = fork (); 
        if (pid == 0) { 
            // child
            close (fd[0]); 
            memset (buf, 'a' + i, pipe_buf * P); 
            for (j=0; j<M; ++ j) { 
                ret = write (fd[1], buf, pipe_buf * P); 
                printf ("child %ld write %d with %c in turn %d\n", getpid (), ret, 'a' + i, j); 
            }

            sleep (i); 
            printf ("child %ld exit\n", getpid ()); 
            return 0; 
        }
        else if (pid > 0) { 
            // parent
            printf ("start child %lu\n", pid); 
        }
        else 
            err_sys ("fork"); 
    }

    close (fd[1]); 
    //for (i=0; i<N * M * P; ++ i) {  
    while (1) { 
        ret = read (fd[0], buf, pipe_buf * P); 
        if (ret == 0) { 
            printf ("reach end\n"); 
            break; 
        }

        char tmp = buf[0]; 
        printf ("parent read %d of %c\n", ret, tmp); 
        for (j=0; j<ret; j++) {
            if (buf[j] != tmp)
                err_sys ("find different character %c at %d\n", buf[j], j); 
        }
    }

    //sleep (20); 
    return 0; 
}
