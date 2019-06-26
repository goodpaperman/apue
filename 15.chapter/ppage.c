#include "../apue.h" 
#include <unistd.h> 
#include <sys/wait.h> 

#define PAGER "${PAGER:-more}"
#if 0
#  define POPEN popen
#  define PCLOSE pclose
#else 
#  define POPEN apue_popen
#  define PCLOSE apue_pclose
#endif 

int main (int argc, char *argv[])
{
    int n = 0; 
    char line[MAXLINE] = { 0 }; 
    FILE *fpin = NULL, *fpout = NULL; 
    if (argc != 2)
        err_quit ("usage: ppage <pathname>"); 

    fpin = fopen (argv[1], "r"); 
    if (fpin == NULL)
        err_sys ("can't open %s", argv[1]); 

    fpout = POPEN (PAGER, "w"); 
    if (fpout == NULL)
        err_sys ("popen %s error", PAGER); 

    while (fgets (line, MAXLINE, fpin) != NULL) { 
        if (fputs (line, fpout) == EOF)
            err_sys ("fputs error to pipe"); 
    }

    if (ferror (fpin))
        err_sys ("fgets error"); 

#if 0
    // to see pipe at parent
    sleep (60); 
#endif 

    int ret = PCLOSE (fpout); 
    if (ret == -1)
        err_sys ("pclose error"); 
    else 
        printf ("worker return %d\n", ret); 

    return 0; 
}
