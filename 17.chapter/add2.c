#include <unistd.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h>

#define MAXLINE 128
#define USE_FSPIPE

int main (void)
{
    int fdin, fdout, n, int1, int2; 
    char line[MAXLINE]; 
#ifdef USE_FSPIPE
	fdin = open ("./pipe", O_RDWR); 
	if (fdin < 0) {
		printf ("open file pipe failed\n"); 
		return 0; 
	}

	printf ("open file pipe ok, fd = %d\n", fdin); 
	fdout = fdin; 
#else 
	fdin = STDIN_FILENO; 
	fdout = STDOUT_FILENO; 
#endif

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
#ifdef USE_FSPIPE
			printf ("i am working on %s\n", line); 
#endif
        }
        else { 
            if (write (fdout, "invalid args\n", 13) != 13) {
                printf ("write msg error\n"); 
				return 0; 
			}
        }
    }

#ifdef USE_FSPIPE
	close (fdin); 
	//close (fdout); 
#endif

    return 0; 
}
