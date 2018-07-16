#include "../apue.h" 
#include <time.h> 


int main (int argc, char *argv[])
{
    time_t start = time (NULL); 
    int ret = alrm_sleep (5); 
    time_t end = time (NULL); 
    printf ("alarm return %d, sleep %u, start %lu, end %lu\n", ret, (int)(end - start), start, end); 
    return 0; 
}
