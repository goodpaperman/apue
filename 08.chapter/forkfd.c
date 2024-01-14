#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
#if 0
    setvbuf (stdout, NULL, _IOFBF, 0);  
    printf ("before fork\n"); 
#elif 0
    printf ("before fork "); 
#else
    printf ("before fork\n"); 
#endif

#ifdef USE_VFORK
    char buf[128] = { 0 };  
    int pid = vfork();
#else
    int pid = fork();
#endif
    if (pid < 0)
    {
        // error
        exit(1);
    }
    else if (pid == 0)
    {
        // child
        printf ("%d spawn from %d\n", getpid(), getppid());
#ifdef USE_VFORK
#  if 1
        fclose (stdin); 
        fclose (stdout); 
        fclose (stderr); 
#  else
        close (STDIN_FILENO); 
        close (STDOUT_FILENO); 
        close (STDERR_FILENO); 
#  endif
        exit(0); 
#endif
    }
    else
    {
        // parent
#ifndef USE_VFORK
        sleep (1);
        printf ("%d create %d\n", getpid(), pid);
#else
        sprintf (buf, "%d create %d\n", getpid(), pid);
        write (STDOUT_FILENO, buf, strlen(buf)); 
#endif
    }

#ifdef USE_VFORK
    sprintf (buf, "after fork\n"); 
    write (STDOUT_FILENO, buf, strlen(buf)); 
#else
    printf ("after fork\n"); 
#endif
    return 0;
}
