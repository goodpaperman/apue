#include "../apue.h"
#include <termios.h> 

#define NEW_BITS 5
#define CS_NEW_BITS CS5

int main (void)
{
    struct termios term; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

    switch (term.c_cflag & CSIZE) {
      case CS5:
        printf ("5 bits/byte\n"); 
        break; 
      case CS6:
        printf ("6 bits/byte\n"); 
        break; 
      case CS7:
        printf ("7 bits/byte\n"); 
        break; 
      case CS8:
        printf ("8 bits/byte\n"); 
        break; 
      default:
        printf ("unknown bits/byte\n"); 
        break; 
    }

    term.c_cflag &= ~CSIZE; 
    term.c_cflag |= CS_NEW_BITS; 

    if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
      err_sys ("tcsetattr error"); 

    printf ("change bits per byte to %d ok\n", NEW_BITS); 
    return 0; 
}
