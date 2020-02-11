#include "../apue.h"
#include "../tty.h"

int main (void)
{
  int i; 
  char c; 
  if (tty_raw (STDIN_FILENO) < 0)
    err_sys ("tty_raw error"); 

  printf ("Enter raw mode, terminate with DELETE\n"); 
  while ((i = read (STDIN_FILENO, &c, 1)) == 1) {
    if ((c &= 255) == 0177)
      break; 

    printf ("%o\n", c); 
    //printf ("%d\n", c); 
  }

  // no tty_reset, to test reset cmd.
  //if (tty_reset (STDIN_FILENO) < 0)
  //  err_sys ("tty_reset error"); 

  printf ("exit\n"); 
  return 0; 
}
