#include <utmp.h> 
#include "../apue.h"

int main (int argc, char *argv[])
{
  struct utmp ut = { 0 }; 
  printf ("sizeof (struct utmp) = %d\n", sizeof(ut)); 
  return 0; 
}
