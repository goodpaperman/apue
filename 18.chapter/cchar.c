#include "../apue.h"
#include <termios.h> 

int main (void)
{
    struct termios term; 
    long vdisable; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    vdisable = fpathconf (STDIN_FILENO, _PC_VDISABLE); 
    if (vdisable < 0)
        err_quit ("fpathconf error or _POSIX_VDISABLE not in effect"); 
    else 
        printf ("VDISABLE = '%c'\n", vdisable); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

    term.c_cc[VINTR] = vdisable; 
    term.c_cc[VEOF] = 2; /* Ctrl+B */

    if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &term) < 0)
      err_sys ("tcsetattr error"); 

    printf ("change special input key ok\n"); 
    return 0; 
}
