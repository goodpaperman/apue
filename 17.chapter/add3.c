#include <unistd.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h>

#define MAXLINE 128

int main (void)
{
    int fdin, fdout, n, int1, int2; 
    char line[MAXLINE]; 
	fdin = open ("./pipe", O_RDWR); 
	if (fdin < 0) {
		printf ("open file pipe failed\n"); 
		return 0; 
	}

	printf ("open file pipe ok, fd = %d\n", fdin); 
	fdout = fdin; 

    while ((n = read (fdin, line, MAXLINE)) > 0) { 
        line[n] = 0; 
        if (sscanf (line, "%d%d", &int1, &int2) == 2) { 
            sprintf (line, "%d\n", int1 + int2); 
            n = strlen (line); 
            if (write (fdout, line, n) != n) {
                printf ("write error\n"); 
				return 0; 
			}
			printf ("i am working on %s\n", line); 
        }
        else { 
            if (write (fdout, "invalid args\n", 13) != 13) {
                printf ("write msg error\n"); 
				return 0; 
			}
        }
    }

	close (fdin); 
	//close (fdout); 

    return 0; 
}
