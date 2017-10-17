#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
#ifdef ENOUGH_BUF
  #define SIZE 1200 
#else 
  #define SIZE 15 
#endif 
  char pad1[SIZE] = { 0 }; 
  char buf[SIZE] = { 0 }; 
  char pad2[SIZE] = { 0 }; 
#ifdef USE_GETS
  if (gets (buf) == 0)
#else
  if (fgets (buf, SIZE, stdin) == 0)
#endif 
  {
    if (feof (stdin))
      printf ("reach EOF, read: %s\n", buf); 
    else if (ferror (stdin))
      printf ("read error\n"); 
    else 
      printf ("read nothing\n"); 
  }
  else 
  {
    printf ("read: %s\n", buf); 
    printf ("pad1: %s\n", pad1); 
    printf ("pad2: %s\n", pad2); 
  }

  return 0; 
}
