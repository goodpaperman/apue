//#include "../apue.h"  
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <fcntl.h>

#define MAXLINE 128
 

int main (int argc, char *argv[])
{
    if (argc < 2) { 
        printf ("usage: padd3 <add3-program>\n"); 
        return 0; 
    }

    int fdin, fdout, n, int1, int2; 
    char line[MAXLINE]; 
	fdin = open ("./pipe", O_RDWR); 
	if (fdin < 0) {
		printf ("open file pipe failed\n"); 
		return 0; 
	}

	printf ("open file pipe ok, fd = %d\n", fdin); 
	fdout = fdin; 

    n = strlen (line); 
    if (write (fdout, line, n) != n){
        printf ("write error to pipe\n"); 
		return 0; 
	}
    if ((n = read (fdin, line, MAXLINE)) < 0) {
        printf ("read error from pipe\n"); 
		return 0; 
	}

    if (n == 0) { 
        printf ("child closed pipe\n"); 
		return 0; 
    }

    line[n] = 0; 
    if (fputs (line, stdout) == EOF) {
        printf ("fputs error\n"); 
		return 0; 
	}

    if (ferror (stdin)) {
        printf ("fputs error\n"); 
		return 0; 
	}

	close (fdin);
    return 0; 
}

