#include <unistd.h>
#include <stdio.h> 
#include <string.h> 

#define MAXLINE 128

int main (void)
{
    int n, int1, int2; 
    char line[MAXLINE]; 
    while ((n = read (STDIN_FILENO, line, MAXLINE)) > 0) { 
        line[n] = 0; 
        if (sscanf (line, "%d%d", &int1, &int2) == 2) { 
            sprintf (line, "%d\n", int1 + int2); 
            n = strlen (line); 
            if (write (STDOUT_FILENO, line, n) != n) {
                printf ("write error\n"); 
				return 0; 
			}
		
        }
        else { 
            if (write (STDOUT_FILENO, "invalid args\n", 13) != 13) {
                printf ("write msg error\n"); 
				return 0; 
			}
        }
    }

    return 0; 
}
