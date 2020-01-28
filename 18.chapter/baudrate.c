#include "../apue.h"
#include <termios.h> 

int main (void)
{
    struct termios term; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

    speed_t s = B50; 
    if (cfsetispeed(&term, s) < 0)
        err_sys ("cfsetispeed to %d failed", s); 

    printf ("set input speed to %d ok\n", s); 

    s = B110; 
    if (cfsetospeed (&term, s) < 0)
        err_sys ("cfsetospeed to %d failed", s); 

    printf ("set output speed to %d ok\n", s); 
    if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
      err_sys ("tcsetattr error"); 

    printf ("set attr ok\n"); 
    return 0; 
}
