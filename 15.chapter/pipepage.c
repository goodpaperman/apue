#include "../apue.h" 
#include <unistd.h> 
#include <sys/wait.h> 

#define DEF_PAGER "/bin/more"

int main (int argc, char *argv[])
{
    int n = 0; 
    char *pager = 0, *argv0 = 0; 
    char line[MAXLINE] = { 0 }; 
    if (argc != 2)
        err_quit ("usage: pipepage <pathname>"); 

    FILE *fp = fopen (argv[1], "r"); 
    if (fp == NULL)
        err_sys ("can't open %s", argv[1]); 

    int fd[2] = { 0 }; 
    if (pipe (fd) < 0)
        err_sys ("pipe error"); 

    pid_t pid = fork (); 
    if (pid < 0) { 
        err_sys ("fork error"); 
    } else if (pid > 0) { 
        // parent
        close (fd[0]); 
        while (fgets (line, MAXLINE, fp) != NULL) { 
            n = strlen (line); 
            if (write (fd[1], line, n) != n)
                err_sys ("write error to pipe"); 
        }

        if (ferror (fp))
            err_sys ("fgets error"); 
        else 
            fclose (fp); 

        // must close pipe to let it know there is no more data !
        // (and can exit with 'q')
        close (fd[1]); 
#if 1
        // must wait child, or child will abort too.
        if (waitpid (pid, NULL, 0) < 0)
            err_sys ("waitpid error"); 
#endif

        //printf ("wait child"); 
    } else { 
        // child
        close (fd[1]); 
        fclose (fp); 

        printf ("child begin to redirect pipe to stdin, pipe fd %d\n", fd[0]); 
        if (fd[0] != STDIN_FILENO) { 
            if (dup2 (fd[0], STDIN_FILENO) != STDIN_FILENO)
                err_sys ("dup2 error to stdin"); 

            close (fd[0]); 
            printf ("redirect OK\n"); 
        }

        pager = getenv("PAGER"); 
        if (pager == NULL)
            pager = DEF_PAGER; 

        argv0 = strrchr (pager, '/'); 
        if (argv0 != NULL)
            argv0++; 
        else 
            argv0 = pager; 

        if (execl (pager, argv0, (char *)0) < 0)
            err_sys ("execl error for %s", pager); 
    }

    return 0; 
}
