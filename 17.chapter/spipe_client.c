#include <unistd.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h>

#define MAXLINE 128

int main (void)
{
    int fdin, fdout, n, int1, int2; 
    char line[MAXLINE]; 
	fdin = STDIN_FILENO; 
	fdout = STDOUT_FILENO; 

    while ((n = read (fdin, line, MAXLINE)) > 0) { 
        line[n] = 0; 
		printf ("source: %s\n", line); 
        if (sscanf (line, "%d%d", &int1, &int2) == 2) { 
            sprintf (line, "%d\n", int1 + int2); 
            n = strlen (line); 
            if (write (fdout, line, n) != n) {
                printf ("write error\n"); 
				return 0; 
			}
        }
        else { 
            if (write (fdout, "invalid args\n", 13) != 13) {
                printf ("write msg error\n"); 
				return 0; 
			}
        }
    }

    return 0; 
}
