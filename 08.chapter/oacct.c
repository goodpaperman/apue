#include <signal.h> 
#include <sys/types.h> 
#include "../apue.h" 

int main (void)
{
    pid_t pid; 
    if ((pid = fork ()) < 0)
        err_sys ("fork error"); 
    else if (pid != 0) {
        printf ("1st child, sleeping 2\n"); 
        sleep (2); 
        printf ("1st child exit\n"); 
        exit (2); 
    }

    if ((pid = fork ()) < 0)
        err_sys ("fork error"); 
    else if (pid != 0) {
        printf ("2nd child, sleeping 4\n"); 
        sleep (4); 
        printf ("2nd child abort\n"); 
        abort (); 
    }

    if ((pid = fork ()) < 0)
        err_sys ("fork error"); 
    else if (pid != 0) {
        printf ("3rd child, dd \n"); 
        //execl ("/usr/bin/dd", "dd", "if=/boot", "of=/dev/null", NULL); 
        execl ("/bin/dd", "dd", "if=/boot/efi/EFI/redhat/grub.efi", "of=/dev/null", NULL); 
        printf ("3rd child exit\n"); 
        exit (7); 
    }

    if ((pid = fork ()) < 0)
        err_sys ("fork error"); 
    else if (pid != 0) {
        printf ("4th child, sleeping 8\n"); 
        sleep (8); 
        printf ("4th child exit\n"); 
        exit (0); 
    }

    printf ("parent sleep 6\n"); 
    sleep (6); 
    printf ("parent kill self\n"); 
    kill (getpid (), SIGKILL); 
    printf ("parent exit\n"); 
    exit (6); 
}
