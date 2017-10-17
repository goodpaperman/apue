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

  ungetc ('O', stdin); 
  printf ("after ungetc\n"); 

  if (feof (stdin))
    printf ("reach EndOfFile\n"); 
  else 
    printf ("not reach EndOfFile\n"); 

  if (ferror (stdin))
    printf ("read error\n"); 
  else 
    printf ("not read error\n"); 

  int i = 0; 
  char ch = 0; 
  for (i=0; i<26; ++ i)
  {
    ch = 'a'+i; 
    if (ungetc (ch, stdin) != ch)
    {
      printf ("ungetc failed\n"); 
      break; 
    }
  }

  if (ungetc (EOF, stdin) == EOF)
    printf ("ungetc EOF failed\n"); 

  while (1)
  {
    ret = getc (stdin); 
    if (ret == EOF)
      break; 

    printf ("read %c\n", (unsigned char) ret); 
  }

  return 0; 
}
