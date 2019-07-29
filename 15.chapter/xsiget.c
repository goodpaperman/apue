#include "../apue.h" 

#define USE_EXCL
#define USE_FTOK

#include <sys/msg.h>
#include <sys/sem.h> 
#include <sys/shm.h> 

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        printf ("Usage: xsiget type <msg|sem|shm> projid\n"); 
        return -1; 
    }

    char const* type = argv[1]; 
    int id = atoi (argv[2]); 

    key_t key = IPC_PRIVATE; 
#ifdef USE_FTOK
    key = ftok ("./xsiget.c", id); 
#endif

    int flag = IPC_CREAT; 
#ifdef USE_EXCL
    flag |= IPC_EXCL; 
#endif

    if (strcasecmp (type, "msg") == 0)
    {
        int ret = msgget (key, flag); 
        if (ret < 0)
            err_sys ("msgget failed"); 

        printf ("create msgqueue %d with key 0x%08x ok\n", ret, key); 
    }
    else if (strcasecmp (type, "sem") == 0)
    {
        int ret = semget (key, 1, flag); 
        if (ret < 0)
            err_sys ("semget failed"); 

        printf ("create semphore %d with key 0x%08x ok\n", ret, key); 
    } 
    else if (strcasecmp (type, "shm") == 0)
    {
        int ret = shmget (key, 1024, flag); 
        if (ret < 0)
            err_sys ("shmget failed"); 

        printf ("create share memory %d with key 0x%08x ok\n", ret, key); 
    }
    else 
        printf ("unknown type %s\n", type); 

    //sleep (60); 
    return 0; 
}
