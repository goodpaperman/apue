#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h> 
#include <time.h> 
#include <set>

int main (int argc, char *argv[])
{
    std::set<pid_t> pids; 
    pid_t pid = getpid();
    time_t start = time(NULL); 
    pids.insert(pid); 
    while (true)
    {
        if ((pid = fork ()) < 0)
        {
            printf ("fork error\n"); 
            return 1; 
        }
        else if (pid == 0)
        {
            printf ("[%u] child running\n", getpid()); 
            break; 
        }
        else 
        {
            printf ("fork and exec child %u\n", pid); 

            int status = 0; 
            pid = wait(&status); 
            printf ("wait child %u return %d\n", pid, status); 
            if (pids.find (pid) == pids.end())
            {
                pids.insert(pid); 
            }
            else 
            {
                time_t end = time(NULL); 
                printf ("duplicated pid find: %u, total %lu, elapse %lu\n", pid, pids.size(), end-start); 
                // printf ("sizeof (pid_t) = %d\n", sizeof (pid_t)); 
                break; 
            }
        }
    }

    exit (0); 
}
