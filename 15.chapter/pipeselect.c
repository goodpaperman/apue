#include "../apue.h" 
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/select.h>
#include <sys/param.h> // for MAX

int main (int argc, char *argv[])
{
    int fd_in[2] = { 0 }; 
    int fd_out[2] = { 0 }; 
    if (pipe (fd_in) < 0)
        err_sys ("pipe stdin error"); 

    if (pipe (fd_out) < 0)
        err_sys ("pipe stdout error"); 

    char line[4096] = { 0 }; 
    int ret = 0, max = MAX(fd_out[0], STDIN_FILENO)+1;  
    printf ("max fd: %d, pipe: %d, stdin: %d\n", max, fd_out[0], STDIN_FILENO); 
    pid_t pid = fork (); 
    if (pid < 0) { 
        err_sys ("fork error"); 
    } else if (pid > 0) { 
        // parent
        close (fd_in[0]); 
        close (fd_out[1]); 
        
        fd_set rfds; 
        FD_ZERO(&rfds); 

        while (1) { 
            // must set it in every loop.
            FD_SET(fd_out[0], &rfds); 
            FD_SET(STDIN_FILENO, &rfds); 
            ret = select (max, &rfds, NULL, NULL, NULL/*&tv*/); 
            // will clear rfds & tv after this call.
		    if (ret == -1) { 
                err_sys ("select error"); 
            } else if (ret == 0){ 
			    printf ("select over but no event\n"); 
                continue; 
            }  

            printf ("got event\n"); 
            if (FD_ISSET(STDIN_FILENO, &rfds)) {
                printf ("select stdin\n"); 
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

            if (FD_ISSET(fd_out[0], &rfds)) {
                printf ("poll pipe in\n"); 
                // has output, redirect to stdout
                ret = read (fd_out[0], line, sizeof (line)); 
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
