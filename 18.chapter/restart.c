#include "../apue.h"
#include <termios.h> 
#include <errno.h>

int main (void)
{
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    if (tcflow (STDIN_FILENO, TCOON) < 0)
        err_quit ("flow stdout to on failed, errno %d\n", errno); 

    printf ("flow stdout to on ok\n"); 
    if (tcflow (STDIN_FILENO, TCION) < 0)
        err_quit ("flow stdin to on failed, errno %d\n", errno); 

    printf ("flow stdin to on ok\n"); 
    return 0; 
}
