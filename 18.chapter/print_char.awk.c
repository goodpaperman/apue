#include "../apue.h" 
#include <termios.h> 

void print_control_char (cc_t *cc, int len)
{
    printf ("input char array size %d\n", len); 
#ifdef VDISCARD
    if (VDISCARD >= 0 && VDISCARD < len)
        printf ("    cc[VDISCARD] = %c\n", cc[VDISCARD]); 
    else 
        printf ("    VDISCARD out of index\n"); 
#else
    printf ("    VDISCARD not defined\n"); 
#endif
}
