#include "../apue.h"
#include <termios.h> 

extern void print_input_flag (tcflag_t flag); 
extern void print_output_flag (tcflag_t flag); 
extern void print_control_flag (tcflag_t flag); 
extern void print_local_flag (tcflag_t flag); 
extern void print_local_char (cc_t* cc, int len); 

int main (void)
{
    struct termios term; 
    if (isatty (STDIN_FILENO) == 0)
        err_quit ("standard input is not a terminal device"); 

    if (tcgetattr (STDIN_FILENO, &term) < 0)
        err_sys ("tcgetattr error"); 

    print_input_flag (term.c_iflag); 
    print_output_flag (term.c_oflag); 
    print_control_flag (term.c_cflag); 
    print_local_flag (term.c_lflag); 
    print_control_char (term.c_cc, NCCS); 
    return 0; 
}
