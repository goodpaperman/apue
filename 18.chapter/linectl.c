#include "../apue.h"
#include <termios.h> 
#include <errno.h>

//#define DRAIN_OUT
#define DRAIN_IN

int main (void)
{
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    printf ("termid: %s, ttyname: %s\n", ctermid(NULL), ttyname(STDIN_FILENO)); 
    if (tcdrain (STDIN_FILENO) < 0)
        err_quit ("drain stdout failed, errno %d\n", errno); 

#ifdef DRAIN_OUT
    printf ("drain stdout ok\n"); 
    if (tcflow (STDIN_FILENO, TCOOFF) < 0)
        err_quit ("flow stdout to off failed, errno %d\n", errno); 

#  if 0
    if (tcflow (STDIN_FILENO, TCOON) < 0)
        err_quit ("flow stdout to on failed, errno %d\n", errno); 
#  endif 

    printf ("flow stdout to off ok\n"); 
#endif

#ifdef DRAIN_IN
    if (tcflow (STDIN_FILENO, TCIOFF) < 0)
        err_quit ("flow stdin to off failed, errno %d\n", errno); 

    printf ("flow stdin to off ok\n"); 
#endif
    
    int n; 
    printf ("please input a numeric value\n"); 
    scanf ("%d", &n); 
    printf ("get %d\n", n); 

#ifdef DRAIN_IN
    if (tcflow (STDIN_FILENO, TCION) < 0)
        err_quit ("flow stdin to on failed, errno %d\n", errno); 

    printf ("flow stdin to on ok\n"); 
#endif 

#ifdef DRAIN_OUT
    if (tcflow (STDIN_FILENO, TCOON) < 0)
        err_quit ("flow stdout to on failed, errno %d\n", errno); 

    printf ("flow stdout to on ok\n"); 
#endif

    if (tcflush (STDIN_FILENO, TCIFLUSH) < 0)
        err_quit ("flush input failed, errno %d\n", errno); 

    printf ("flush input ok\n"); 
    if (tcflush (STDIN_FILENO, TCOFLUSH) < 0)
        err_quit ("flush output failed, errno %d\n", errno); 

    printf ("flush output ok\n"); 
    if (tcflush (STDIN_FILENO, TCIOFLUSH) < 0)
        err_quit ("flush input & output failed, errno %d\n", errno); 

    printf ("flush input & output ok\n"); 

    if (tcsendbreak (STDIN_FILENO, 0) < 0)
        err_quit ("send break failed, errno %d\n", errno); 

    printf ("send default break ok\n"); 

    int dur = 10; 
    if (tcsendbreak (STDIN_FILENO, dur) < 0)
        err_quit ("send break with %d failed, errno %d\n", dur, errno); 

    printf ("send break with %d ok\n", dur); 
    return 0; 
}
