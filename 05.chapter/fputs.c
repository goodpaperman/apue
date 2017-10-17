#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  char const* str = "this is a string without carrier return"; 
  char const* str_n = "this is a line with carrier return\n"; 
  if (puts (str) == EOF)
    printf ("puts failed\n"); 
  else 
    printf ("puts OK\n"); 

  if (puts (str_n) == EOF)
    printf ("puts failed\n"); 
  else 
    printf ("puts OK\n"); 

  if (fputs (str, stdout) == EOF)
    printf ("fputs failed\n"); 
  else 
    printf ("fputs OK\n"); 

  if (fputs (str_n, stdout) == EOF)
    printf ("fputs failed\n"); 
  else 
    printf ("fputs OK\n"); 

  return 0; 
}
