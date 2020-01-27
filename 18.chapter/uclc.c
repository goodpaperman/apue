#include "../apue.h"
#include <termios.h> 

#define UC2LC

int main (void)
{
    struct termios term; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

#ifdef UC2LC
    if (term.c_iflag & IUCLC) 
        printf ("no upper char to lower char convert\n"); 
    else
        printf ("has upper char to lower char convert\n"); 

    term.c_iflag |= IUCLC; 
#else
    if (term.c_oflag & OLCUC) 
        printf ("no lower char to upper char convert\n"); 
    else
        printf ("has lower char to upper char convert\n"); 

    term.c_oflag |= OLCUC; 
#endif

    if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
      err_sys ("tcsetattr error"); 

#ifdef UC2LC
    printf ("upper char to lower char convert ok\n"); 
#else
    printf ("lower char to upper char convert ok\n"); 
#endif
    return 0; 
}
