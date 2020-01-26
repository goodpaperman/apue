#include "../apue.h"
#include <termios.h> 

//#define DISABLE_ON

int main (void)
{
    long vdisable; 
    struct termios term; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    vdisable = fpathconf (STDIN_FILENO, _PC_VDISABLE); 
    if (vdisable < 0)
        err_quit ("fpathconf error or _POSIX_VDISABLE not in effect"); 
    else 
        printf ("VDISABLE = '%c'\n", vdisable); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

    if (term.c_iflag & IXON) 
        printf ("IXON\n"); 
    else
        printf ("no IXON\n"); 

    if (term.c_iflag & IXANY) 
        printf ("IXANY\n"); 
    else
        printf ("no IXANY\n"); 

#ifdef DISABLE_ON
    term.c_iflag &= ~IXON; 
#else
    term.c_iflag &= ~IXANY; 
#  if 0
    term.c_cc[VSTOP] = vdisable; 
    term.c_cc[VSTART] = vdisable; 
#  else
    term.c_cc[VSTOP] = 16; /* Ctrl+P */
    term.c_cc[VSTART] = 20; /* Ctrl+T */
#  endif
#endif

    if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
      err_sys ("tcsetattr error"); 

#ifdef DISABLE_ON
    printf ("clear IXON ok\n"); 
#else
    printf ("clear IXANY and (disable STOP or set START to ^T) ok\n"); 
#endif
    return 0; 
}
