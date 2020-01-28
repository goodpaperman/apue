#include "../apue.h" 
#include <termios.h> 

void print_speed (char const* prompt, speed_t s)
{
    printf ("%s %d\n", prompt, s); 
    switch (s)
    {
#ifdef B50
    case B50:
        printf ("%s bits/s\n", "B50"+1); 
        break; 
#endif
    default:
        printf ("unknown enum %d\n", s); 
        break; 
    }
}
