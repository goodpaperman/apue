#include "../apue.h" 
#include <unistd.h> 
#include <sys/stat.h>

#define N 3
#define PARENT_WRITE

int main (int argc, char *argv[])
{
    int ret = 0, i = 0; 
    char buf[3] = { 0 }; 
    int fd[2] = { 0 }; 
    if (pipe(fd) < 0) 
        err_sys ("pipe"); 

    printf ("parent %lu start\n", getpid ()); 

    for (i=0; i<N; ++ i) { 
        pid_t pid = fork (); 
        if (pid == 0) { 
            // child
#ifdef PARENT_WRITE
            close (fd[1]); 
            ret = read (fd[0], buf, 1); 
            if (ret == 0) { 
                printf ("reach end\n"); 
                return 0; 
            }

            printf ("child %ld read %c\n", getpid (), buf[0]); 
            sleep (1); 
            ret = read (fd[0], buf, 1); 
            if (ret == 0) { 
                printf ("reach end\n"); 
                return 0; 
            }

            printf ("child %ld read %c\n", getpid (), buf[0]); 

#  if 0
            sleep (1); 
            // add 1 to read more and block
            ret = read (fd[0], buf, 1); 
            if (ret == 0) { 
                printf ("reach end\n"); 
                return 0; 
            }

            printf ("child %ld read %c\n", getpid (), buf[0]); 
#  endif 
#else 
            close (fd[0]); 
            buf[0] = 'a' + i; 
            buf[1] = 'A' + i; 
            buf[2] = 0; 
            ret = write (fd[1], buf, 2); 
            printf ("child %ld write %d with %c\n", getpid (), ret, 'a' + i); 
#endif
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

#ifdef PARENT_WRITE
       close (fd[0]); 
       for (i=0; i<N + 1; ++ i) { // add 1 to write more and crash
           sprintf (buf, "%c%c", 'a' + i, 'A' + i); 
           ret = write (fd[1], buf, 2); 
           sleep (2); 
           printf ("parent write %d with %c\n", ret, 'a' + i); 
       }
#else
       close (fd[1]); 
       for (i=0; i<N+1; ++ i) {  // add 1 to read more and block
           ret = read (fd[0], buf, 1); 
           if (ret == 0) { 
               printf ("reach end\n"); 
               break; 
           }

           printf ("parent read %c\n", buf[0]); 
           //sleep (1); 
           ret = read (fd[0], buf, 1); 
           if (ret == 0) { 
               printf ("reach end\n"); 
               break; 
           }

           printf ("parent read %c\n", buf[0]); 
       }
#endif

    sleep (20); 
    return 0; 
}
