#include "../apue.h"
#include <sys/types.h> 
#include <sys/file.h> 
#include <sys/stat.h> 
#include <unistd.h> 

void print_ids (uid_t ouid)
{
#ifdef __APPLE__
    uid_t ruid = getuid(); 
    uid_t euid = geteuid(); 
    printf ("%d: ruid %d, euid %d\n", getpid(), ruid, euid); 
#else
    uid_t ruid = 0; 
    uid_t euid = 0; 
    uid_t suid = 0;
    int ret = getresuid (&ruid, &euid, &suid); 
    if (ret == 0)
    {
        printf ("%d: ruid %d, euid %d, suid %d, ouid %d\n", getpid(), ruid, euid, suid, ouid); 
        if (ouid != 0 && ruid == euid && euid == suid && suid != ouid)
        {
            printf ("all uid same %d, change back to old %d\n", ruid, ouid); 
            ret = seteuid (ouid); 
            if (ret != 0)
                err_sys ("seteuid"); 
            else 
                print_ids (0); 
        }
    }
    else 
        err_sys ("getresuid"); 
#endif
}

int main (int argc, char *argv[])
{
    if (argc == 2)
    {
        char* uid=argv[1]; 
        int ret = setuid(atol(uid)); 
        if (ret != 0)
            err_sys ("setuid"); 

        print_ids(0); 
    }
    else if (argc == 3)
    {
        char* ruid=argv[1]; 
        char* euid=argv[1]; 
        uid_t ouid = getuid(); 
        int ret = setreuid(atol(ruid), atol(euid)); 
        if (ret != 0)
            err_sys ("setreuid"); 

        // to test if ruid/euid/suid changed to same
        // can we change back again?
        print_ids(ouid); 
    }
    else if (argc > 1)
    {
        char* uid=argv[1]; 
        int ret = seteuid(atol(uid)); 
        if (ret != 0)
            err_sys ("seteuid"); 

        print_ids(0); 
    }
    else 
    {
        print_ids(0); 
    }

    return 0; 
}


