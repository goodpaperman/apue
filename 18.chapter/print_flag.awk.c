#include "../apue.h" 
#include <termios.h> 

void print_input_flag (tcflag_t flag)
{
    printf ("input flag %p\n", flag); 
#ifdef CLOCAL
    if (flag & CLOCAL)
        printf ("   CLOCAL\n"); 
#endif
}
