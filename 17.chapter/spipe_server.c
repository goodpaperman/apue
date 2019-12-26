//#include "../apue.h" 
#include <signal.h> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "spipe_fd.h"

#define MAXLINE 128

int s_pipe (int fd[2])
{
	// on solaris
	return (pipe(fd)); 
}

static void sig_pipe (int); 

int get_temp_fd ()
{
	char fname[128] = "/tmp/outXXXXXX"; 
	int fd = mkstemp (fname); 
	printf ("create temp file %s with fd %d\n", fname, fd); 
	return fd; 
}

int main (int argc, char *argv[])
{
    if (argc < 2) { 
        printf ("usage: padd2 <add2-program>\n"); 
        return 0; 
    }

    int n;
    if (signal (SIGPIPE, sig_pipe) == SIG_ERR) {
        printf ("signal error\n"); 
		return 0; 
	}

	int fd[2], fd_to_send, fd_to_recv; 
    if (s_pipe (fd) < 0) {
        printf ("pipe error\n"); 
		return 0; 
	}
	printf ("create pipe %d.%d\n", fd[0], fd[1]); 

    char line[MAXLINE]; 
    pid_t pid = fork (); 
    if (pid < 0) {
        printf ("fork error\n"); 
		return 0; 
	}
    else if (pid > 0)
    {
        close (fd[1]); 
        while (fgets (line, MAXLINE, stdin) != NULL) { 
            n = strlen (line); 
			// create temp file and write requet into it !
			fd_to_send = get_temp_fd (); 
			if (fd_to_send < 0) {
				printf ("get temp fd failed\n"); 
				return 0; 
			}
			
            if (write (fd_to_send, line, n) != n){
                printf ("write error to file\n"); 
				return 0; 
			}

#if 0
			if (fsync (fd_to_send) < 0)
				printf ("sync file failed\n"); 
			else 
				printf ("sync data to file\n"); 
#endif

#if 1
			if (lseek (fd_to_send, 0, SEEK_SET) < 0)
				printf ("seek to begin failed\n"); 	
			else
				printf ("seek to head\n"); 
#endif

			n = send_fd (fd[0], fd_to_send); 
			// after send, fd_to_send is close automatically 
			if (n < 0) { 
				printf ("send fd to peer failed, error %d\n", n); 
				return -1; 
			}
			else 
				printf ("send fd %d to peer\n", fd_to_send); 

			fd_to_recv = recv_fd (fd[0], write); 
			if (fd_to_recv < 0) {
				printf ("recv fd from peer failed, error %d\n", fd_to_recv); 
				return -1; 
			}
			else 
				printf ("recv fd %d from peer, position %u\n", fd_to_recv, tell(fd_to_recv)); 	

			// read response by receving the new fd!
            if ((n = read (fd_to_recv, line, MAXLINE)) < 0) {
                printf ("read error from file\n"); 
				return 0; 
			}

			close (fd_to_recv); 
            if (n == 0) { 
                printf ("child closed pipe\n"); 
                break;
            }

            line[n] = 0; 
            if (fputs (line, stdout) == EOF) {
                printf ("fputs error\n"); 
				return 0; 
			}
        }

        if (ferror (stdin)) {
            printf ("fputs error\n"); 
			return 0; 
		}

        return 0; 
    }
    else { 
        close (fd[0]); 
        if (fd[1] != STDIN_FILENO) { 
            if (dup2 (fd[1], STDIN_FILENO) != STDIN_FILENO) {
                printf ("dup2 error to stdin\n"); 
				return 0; 
			}
            //close (fd[0]); 
        }

        if (fd[1] != STDOUT_FILENO) { 
            if (dup2 (fd[1], STDOUT_FILENO) != STDOUT_FILENO) {
                printf ("dup2 error to stdout\n"); 
				return 0; 
			}
            close (fd[1]); 
        }

        if (execl (argv[1], argv[1], (char *)0) < 0) {
            printf ("execl error\n"); 
			return 0; 
		}
    }

    return 0; 
}

static void sig_pipe (int signo)
{
    printf ("SIGPIPE caught\n"); 
    exit (1); 
}
