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
#ifdef TEST_CHANGE_BACK
        if (ouid != -1) 
        {
            printf ("%d: ruid %d, euid %d, suid %d, ouid %d\n", getpid(), ruid, euid, suid, ouid); 
            if (ruid == euid && euid == suid && suid != ouid)
            {
                printf ("all uid same %d, change back to old %d\n", ruid, ouid); 
                ret = seteuid (ouid); 
                if (ret != 0)
                    err_sys ("seteuid"); 
                else 
                    print_ids (-1); 
            }
        }
        else 
#endif
            printf ("%d: ruid %d, euid %d, suid %d\n", getpid(), ruid, euid, suid); 

#ifdef TEST_CHANGE_ANY_USER_WHEN_ALL_ROOT
        if (ruid == 0 && euid == 0)
        {
            // in root, try setreuid(foo, bar)
            int ret = setreuid(1003, 1004); 
            if (ret != 0)
                err_sys ("setreuid"); 
            else 
                print_ids (-1); 
        }
#endif
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
        uid_t ouid = geteuid(); 
        int ret = setuid(atol(uid)); 
        if (ret != 0)
            err_sys ("setuid"); 

        print_ids(ouid); 
    }
    else if (argc == 3)
    {
        char* ruid=argv[1]; 
        char* euid=argv[2]; 
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

        print_ids(-1); 
    }
    else 
    {
        print_ids(-1); 
    }

    return 0; 
}


