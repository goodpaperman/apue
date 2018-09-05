#include "../apue.h" 

void sig_alrm (int signo)
{
    printf ("catch %d\n", signo); 
}

int main (int argc, char *argv[])
{
    if (argc < 2)
        err_sys ("Usage: copy filename\n"); 

    signal (SIGALRM, sig_alrm); 
    alarm (1); 
    char const* path = argv[1]; 
    FILE* fp = fopen (path, "w"); 
    if (fp == NULL)
        err_sys ("fopen error"); 

    int len = 600*1024*1024; 
    char *buf = (char *)malloc (len); 
    if (buf == NULL)
        err_sys ("malloc error"); 

    int ret = fwrite (buf, 1, len, fp); 
    printf ("write %d, return %d\n", len, ret); 
    free (buf); 
    fclose (fp); 
    return 0; 
}
