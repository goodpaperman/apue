#include "../apue.h" 

int main (int argc, char *argv[])
{
  char name [L_tmpnam], line [MAXLINE]; 
  FILE *fp = 0; 
  printf ("%s\n", tmpnam (NULL)); 
  tmpnam (name); 
  printf ("%s\n", name); 
  if ((fp = tmpfile ()) == NULL)
    err_sys ("tmpfile error"); 

  fputs ("one line of output\n", fp); 
  rewind (fp); 
  if (fgets (line, sizeof (line), fp) == NULL)
    err_sys ("fgets error"); 

  fputs (line, stdout); 

  if (argc != 3)
    err_quit ("usage: tmpnam <directory> <prefix>"); 

  char *ptr = tempnam (argv[1][0] != ' ' ? argv[1] : NULL, 
                       argv[2][0] != ' ' ? argv[2] : NULL); 
  printf ("%s\n", ptr); 
  free (ptr); 
  return 0; 
}

