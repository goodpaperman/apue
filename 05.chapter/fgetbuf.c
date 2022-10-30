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
  setvbuf (stdout, buf, _IOFBF, 2048); 
  // fprintf (stdout, "a = %d\n", a); 
  // fflush (stdout); 
  tell_buf ("stdout (full, 2048)", stdout); 
  setvbuf (stderr, buf, _IOLBF, 1024); 
  // fprintf (stderr, "a = %d\n", a); 
  tell_buf ("stderr (line, 1024)", stderr); 
  setvbuf (stdout, NULL, _IOLBF, 4096); 
  // fprintf (stdout, "a = %d\n", a); 
  tell_buf ("stdout (line null 4096)", stdout); 
  setvbuf (stderr, NULL, _IOFBF, 3072); 
  // fprintf (stderr, "a = %d\n", a); 
  // fflush (stderr); 
  tell_buf ("stderr (full null 3072)", stderr); 
  setvbuf (stdout, NULL, _IOFBF, 0); 
  // fprintf (stdout, "a = %d\n", a); 
  // fflush (stdout); 
  tell_buf ("stdout (full null 0)", stdout); 
  setvbuf (stderr, NULL, _IOLBF, 0); 
  // fprintf (stderr, "a = %d\n", a); 
  tell_buf ("stderr (line null 0)", stderr); 
  return 0; 
}
