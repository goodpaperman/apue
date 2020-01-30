#include "../apue.h"

//#define ISATTY isatty

int my_isatty (int fd)
{
  struct termios ts; 
  return tcgetattr (fd, &ts) != -1; 
}

#define ISATTY my_isatty

int main ()
{
  int i = 0; 
  for (i=0; i<3; ++i)
    printf ("fd %d: %s\n", i, ISATTY (i) ? "tty" : "not a tty"); 

  return 0;
}
