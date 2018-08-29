#include "../apue.h" 
#include <time.h>

int main (int argc, char *argv[])
{
    while (1)
    {
        time_t t0 = time (NULL); 
        struct tm *t = localtime (&t0); 
        char buf[128] = { 0 }; 
        strftime (buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t); 
        printf ("%s\n", buf); 
        sleep (60); 
    }
}
