#include "../apue.h" 
#include <unistd.h> 
#include <sys/wait.h> 
#include <poll.h> 

int main (int argc, char *argv[])
{
    int fd_in[2] = { 0 }; 
    int fd_out[2] = { 0 }; 
    if (pipe (fd_in) < 0)
        err_sys ("pipe stdin error"); 

    if (pipe (fd_out) < 0)
        err_sys ("pipe stdout error"); 

    int ret = 0; 
    char line[4096] = { 0 }; 
    pid_t pid = fork (); 
    if (pid < 0) { 
        err_sys ("fork error"); 
    } else if (pid > 0) { 
        // parent
        close (fd_in[0]); 
        close (fd_out[1]); 
        // read pipe from stdin
        // write pipe to stdout/stderr
        //dup2 (fd_in[0], STDIN_FILENO); 
        //dup2 (fd_out[1], STDOUT_FILENO); 
        //dup2 (fd_out[1], STDERR_FILENO); 
        //waitpid (pid, NULL, 0); 
        
	    struct pollfd pfd[2]; 
	    pfd[0].fd = STDIN_FILENO; 
	    pfd[0].events = POLLIN;
	    pfd[1].fd = fd_out[0]; 
	    pfd[1].events = POLLIN; //| POLLHUP;   
        while (1) { 
		    ret = poll (pfd, 2, -1); 
		    if (ret == -1) { 
                err_sys ("poll error"); 
            } else if (ret == 0){ 
			    printf ("poll over but no event\n"); 
                continue; 
            }  

            if (pfd[0].revents & POLLIN) { 
                printf ("poll stdin\n"); 
                // has input, redirect to pipe
                ret = read (STDIN_FILENO, line, sizeof (line)); 
                if (ret < 0)
                    err_sys ("read stdin error"); 
                else if (ret == 0) { 
                    printf ("read end\n"); 
                    break; 
                }
                else {
                    ret = write (fd_in[1], line, ret); 
                    if (ret < 0)
                        err_sys ("write pipe error"); 
                    else if (ret == 0) { 
                        printf ("write end\n"); 
                        break; 
                    }
                    //else 
                    //    printf ("write pipe %d\n", ret); 
                }
            }

            if (pfd[1].revents & POLLIN) { 
                printf ("poll pipe in\n"); 
                // has output, redirect to stdout
                ret = read (pfd[1].fd, line, sizeof (line)); 
                if (ret < 0)
                    err_sys ("read pipe error"); 
                else if (ret == 0) { 
                    printf ("read end\n"); 
                    break; 
                }
                else { 
                    ret = write (STDOUT_FILENO, line, ret); 
                    if (ret < 0)
                        err_sys ("write stdout error"); 
                    else if (ret == 0) { 
                        printf ("write end\n"); 
                        break; 
                    }
                    //else { 
                    //    printf ("write %d\n", ret); 
                    //}
                }
            }

            if (pfd[1].revents & POLLHUP) {
                printf ("poll pipe hup\n"); 
                break; 
            }
        }
    } else { 
        // child
        close (fd_in[1]); 
        close (fd_out[0]); 

#if 1
        printf ("child begin to redirect pipe to stdin/out/err\n"); 
        // read stdin from pipe
        // write stdout/stderr to pipe
        if (fd_in[0] != STDIN_FILENO) { 
            if (dup2 (fd_in[0], STDIN_FILENO) != STDIN_FILENO)
                err_sys ("dup2 stdin error"); 
            close (fd_in[0]); 
        }

        if (fd_out[1] != STDOUT_FILENO) { 
            if (dup2 (fd_out[1], STDOUT_FILENO) != STDOUT_FILENO)
                err_sys ("dup2 stdout error"); 

            // close fd_out[1] later
        }

        if (fd_out[1] != STDERR_FILENO) { 
            if (dup2 (fd_out[1], STDERR_FILENO) != STDERR_FILENO)
                err_sys ("dup2 stderr error"); 

            close (fd_out[1]); 
        }

        printf ("redirect OK\n"); 
        while (gets (line)) { 
            if (strncmp (line, "exit", 4) == 0) { 
                //err_sys ("got exit");  // will write pipe back
                break; 
            }

            puts (line); 
        }
#else
        //execl ("/bin/ls", "ls", NULL); 
        while ((ret = read (fd_in[0], line, sizeof(line))) >= 0) { 
            if (strncmp (line, "exit", 4) == 0) { 
                //err_sys ("got exit");  // will write pipe back
                break; 
            }

            write (fd_out[1], line, ret); 
        }
#endif
    }

    return 0; 
}
