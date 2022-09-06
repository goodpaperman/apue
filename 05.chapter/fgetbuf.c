#include "../apue.h"
#include <stdio.h> 


int main (int argc, char* argv[])
{
  tell_buf ("stdin", stdin); 

  int a; 
  scanf ("%d", &a); 
  printf ("a = %d\n", a); 
  tell_buf ("stdin", stdin); 
  tell_buf ("stdout", stdout); 
  tell_buf ("stderr", stderr); 
  fprintf (stderr, "a = %d\n", a); 
  tell_buf ("stderr", stderr); 
  printf ("\n"); 

  char buf[BUFSIZ] = { 0 }; 
  printf ("bufsiz = %d, address = %p\n", BUFSIZ, buf); 
  setbuf (stdout, NULL); 
  tell_buf ("stdout (no)", stdout); 
  setbuf (stderr, buf); 
  tell_buf ("stderr (has)", stderr); 
  setbuf (stdin, buf); 
  tell_buf ("stdin (has)", stdin); 
  printf ("\n"); 

  setvbuf (stderr, NULL, _IONBF, 0); 
  tell_buf ("stderr (no)", stderr); 
  setvbuf (stdout, buf, _IOLBF, 2048); 
  tell_buf ("stdout (line)", stdout); 
  setvbuf (stderr, buf, _IOFBF, 1024); 
  tell_buf ("stderr (full)", stderr); 
  setvbuf (stdout, NULL, _IOLBF, 4096); 
  printf ("a = %d\n", a); 
  tell_buf ("stdout (line null)", stdout); 
  setvbuf (stderr, NULL, _IOFBF, 3072); 
  fprintf (stderr, "a = %d\n", a); 
  fflush (stderr); 
  tell_buf ("stderr (full null)", stderr); 
  setvbuf (stdout, NULL, _IOLBF, 0); 
  printf ("a = %d\n", a); 
  tell_buf ("stdout (line 0)", stdout); 
  setvbuf (stderr, NULL, _IOFBF, 0); 
  fprintf (stderr, "a = %d\n", a); 
  fflush (stderr); 
  tell_buf ("stderr (full 0)", stderr); 
  return 0; 
}
