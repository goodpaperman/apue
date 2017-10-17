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

  //ungetc ('O', stdin); 
  if (fseek (stdin, 13, SEEK_SET) == -1)
    printf ("fseek failed\n"); 
  else 
    printf ("fseek to 13\n"); 

  printf ("after fseek\n"); 
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
    else 
      printf ("ungetc %c\n", ch); 
  }

  if (fseek (stdin, 20, SEEK_SET) == -1)
    printf ("fseek failed\n"); 
  else 
    printf ("fseek to 20\n"); 

  while (1)
  {
    ret = getc (stdin); 
    if (ret == EOF)
      break; 

    printf ("read %c\n", (unsigned char) ret); 
  }

  return 0; 
}
