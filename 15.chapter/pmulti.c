#include "../apue.h" 
#include <unistd.h> 
#include <sys/wait.h> 

#if 0
#  define POPEN popen
#  define PCLOSE pclose
#else 
#  define POPEN apue_popen
#  define PCLOSE apue_pclose
#endif 

void read_fp (FILE* fp) { 
    char line[MAXLINE] = { 0 }; 
    while (fgets (line, MAXLINE, fp) != NULL) { 
        if (fputs (line, stdout) == EOF)
            err_sys ("fputs error to stdout"); 
    }

    if (ferror (fp))
        err_sys ("fgets error"); 
}


int main (int argc, char *argv[])
{
    int n = 0; 
    FILE *fp1 = NULL, *fp2 = NULL; 
    fp1 = POPEN ("ls -lhrt", "r"); 
    if (fp1 == NULL)
        err_sys ("can't open %s", "ls"); 

    fp2 = POPEN ("ps u", "r"); 
    if (fp2 == NULL)
        err_sys ("can't open %s", "ps"); 

    read_fp (fp1); 
    read_fp (fp2); 

#if 0
    // to see pipe at parent
    sleep (60); 
#endif 

    int ret = PCLOSE (fp1); 
    if (ret == -1)
        err_sys ("pclose error"); 
    else 
        printf ("worker return %d\n", ret); 

    ret = PCLOSE (fp2); 
    if (ret == -1)
        err_sys ("pclose error"); 
    else 
        printf ("worker return %d\n", ret); 

    return 0; 
}
