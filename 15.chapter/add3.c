#include "../apue.h" 

int main (void)
{
    int int1, int2; 
    char line[MAXLINE]; 
#ifdef SETVBUF
    // to see if parent can set them 
    if (setvbuf (stdin, NULL, _IOLBF, 0) != 0)
        err_sys ("setvbuf error"); 
    if (setvbuf (stdout, NULL, _IOLBF, 0) != 0)
        err_sys ("setvbuf error"); 
#endif

    while (fgets(line, MAXLINE, stdin) != NULL) { 
        if (sscanf (line, "%d%d", &int1, &int2) == 2) { 
            if (printf ("%d\n", int1 + int2) == EOF)
                err_sys ("printf error"); 
        }
        else { 
            if (printf("invalid args\n") == EOF)
                err_sys ("printf msg error"); 
        }
    }

    return 0; 
}
