#include "../apue.h"
#include <termios.h> 

int main (void)
{
    struct termios term; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

    if (term.c_iflag & IUCLC) 
        printf ("no upper char to lower char convert\n"); 
    else
        printf ("has upper char to lower char convert\n"); 

    term.c_iflag |= IUCLC; 

    if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
      err_sys ("tcsetattr error"); 

    printf ("upper char to lower char convert ok\n"); 
    return 0; 
}
