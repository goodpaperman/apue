#include "../apue.h"
#include <sys/types.h> 
#include <sys/file.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <fcntl.h> 

void print_ids (char const* prompt)
{
#ifdef __APPLE__
    uid_t ruid = getuid(); 
    uid_t euid = geteuid(); 
    printf ("%s %d: ruid %d, euid %d\n", prompt, getpid(), ruid, euid); 
#else
    uid_t ruid = 0; 
    uid_t euid = 0; 
    uid_t suid = 0;
    int ret = getresuid (&ruid, &euid, &suid); 
    if (ret == 0)
        printf ("%s %d: ruid %d, euid %d, suid %d\n", prompt, getpid(), ruid, euid, suid); 
    else 
        err_sys ("getresuid"); 
#endif
}

int main (int argc, char *argv[])
{
    uid_t ruid = getuid(); 
    uid_t euid = geteuid(); 
    if (ruid == euid)
    {
        printf ("ruid %d != euid %d, please set me set-uid before run this test !\n", ruid, euid); 
        exit (1); 
    }

    print_ids ("init"); 

    int pid = fork (); 
    if (pid < 0)
        err_sys ("fork"); 
    else if (pid == 0)
    {
        // children 
        print_ids ("after fork"); 
#if 1
        int ret = seteuid (ruid); 
        if (ret == -1)
            err_sys ("seteuid"); 

        print_ids ("before exec"); 
#endif 

#if 1
        execlp ("./setuid", "setuid", NULL); 
#else
        char tmp[128] = { 0 }; 
        sprintf (tmp, "%u", euid); 
        execlp ("./setuid", "setuid", tmp, "noop", "noop", NULL); 
#endif
        err_sys ("execlp"); 
    }
    else 
        printf ("create child %u\n", pid); 

    wait(NULL); 
    print_ids("exit"); 
    return 0; 
}


