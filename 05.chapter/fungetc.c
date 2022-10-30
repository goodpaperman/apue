#include "../apue.h"

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
  ungetc ('1', stdin); 
  ungetc ('2', stdin); 
  ungetc ('3', stdin); 
  ungetc ('4', stdin); 
  ungetc ('5', stdin); 
  ungetc ('6', stdin); 
  ungetc ('7', stdin); 
  ungetc ('8', stdin); 
  ungetc ('9', stdin); 
  printf ("after ungetc\n"); 

  if (feof (stdin))
    printf ("reach EndOfFile\n"); 
  else 
    printf ("not reach EndOfFile\n"); 

  if (ferror (stdin))
    printf ("read error\n"); 
  else 
    printf ("not read error\n"); 

  unsigned long long i = 0; 
  char ch = 0; 
  while (1)
  {
    ch = 'a' + i % 26; 
    if (ungetc (ch, stdin) < 0)
    {
      printf ("ungetc %c failed\n", ch); 
      break; 
    }
    ++ i; 
    if (i % 100000000 == 0)
        printf ("unget %llu: %c\n", i, ch); 
  }

  printf ("unget %llu chars\n", i); 
  if (ungetc (EOF, stdin) == EOF)
    printf ("ungetc EOF failed\n"); 

  while (1)
  {
    ret = getc (stdin); 
    if (ret == EOF)
      break; 

    if (i % 100000000 == 0 || i <  30)
        printf ("read %llu: %c\n", i, (unsigned char) ret); 

    // prevent unsigned overflow
    if (i > 0)
        --i; 
  }

  printf ("over!\n"); 
  return 0; 
}
