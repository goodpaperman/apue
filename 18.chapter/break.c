#include "../apue.h"
#include <termios.h> 

int main (void)
{
    struct termios term; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

    if (term.c_iflag & IGNBRK) 
        printf ("ignore break\n"); 
    else
        printf ("handle break (generate int or read as data)\n"); 

    term.c_iflag |= IGNBRK; 

    if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
      err_sys ("tcsetattr error"); 

    printf ("ignore break ok\n"); 
    return 0; 
}
