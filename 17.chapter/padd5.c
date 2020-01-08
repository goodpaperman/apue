//#include "../apue.h"  
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <fcntl.h>
#include <stddef.h> 
#if defined(__sun) || defined(sun) 
#  include <stropts.h>
#  include <sys/stat.h>
#endif

#include "spipe.h"
#define MAXLINE 128

int main (int argc, char *argv[])
{
    if (argc != 1 && argc != 3) { 
        printf ("usage: padd3 <int1> <int2>\n"); 
        return 0; 
    }

    int fdin, fdout, n, m; 
    char line[MAXLINE]; 
	fdin = cli_conn ("./pipe"); 
	if (fdin < 0)
		return 0; 

	printf ("open file pipe ok, fd = %d\n", fdin); 
	fdout = fdin; 

	if (argc == 1) {
    	if ((n = read (STDIN_FILENO, line, MAXLINE)) < 0) {
        	printf ("read error from stdin\n"); 
			return 0; 
		}
	}
	else {
		sprintf (line, "%s %s", argv[1], argv[2]); 
	}

    n = strlen (line); 
    if (write (fdout, line, n) != n){
        printf ("write error to pipe\n"); 
		return 0; 
	}

	strcat (line, " = "); 
	m = strlen (line); 
    if ((n = read (fdin, line + m, MAXLINE - m)) < 0) {
        printf ("read error from pipe\n"); 
		return 0; 
	}

    if (n == 0) { 
        printf ("child closed pipe\n"); 
		return 0; 
    }

    line[m+n] = 0; 
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

