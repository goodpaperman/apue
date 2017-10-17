#include "../apue.h"
#include <wchar.h> 

int main (int argc, char* argv[])
{
#ifdef USE_S
  char buf[MAXLINE] = { 0 }; 
  while (fgets (buf, MAXLINE, stdin) != NULL)
    if (fputs (buf, stdout) == EOF)
      err_sys ("output error"); 
#else
  int c = 0; 
#  ifdef USE_F
  while ((c = fgetc(stdin)) != EOF)
    if (fputc (c, stdout) == EOF)
#  else 
  while ((c = getc(stdin)) != EOF)
    if (putc (c, stdout) == EOF)
#  endif
      err_sys ("output error"); 
#endif

  if (ferror (stdin))
    err_sys ("input error"); 

  return 0; 
}
