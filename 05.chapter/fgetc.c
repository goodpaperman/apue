#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
  int ret = 0; 
  while (1)
  {
    ret = getc (stdin); 
    if (ret == EOF)
      break; 

    printf ("read %c\n", (unsigned char) ret); 
  }

  if (feof (stdin))
    printf ("reach EndOfFile\n"); 
  else 
    printf ("not reach EndOfFile\n"); 

  if (ferror (stdin))
    printf ("read error\n"); 
  else 
    printf ("not read error\n"); 

  clearerr (stdin); 
  printf ("after clear error state\n"); 

  if (feof (stdin))
    printf ("reach EndOfFile\n"); 
  else 
    printf ("not reach EndOfFile\n"); 

  if (ferror (stdin))
    printf ("read error\n"); 
  else 
    printf ("not read error\n"); 

  return 0; 
}
